/* Included after handle_keypress. Actions follow the selected profile. */
#ifndef AVP_PAD_TEST
#include <orbis/Pad.h>
#include "iofocus.h"
#endif
#include "stick.h"
static int ps4_pad_gameplay;
static float ps4_move_x, ps4_move_y, ps4_look_x, ps4_look_y;

static void ps4_poll_pad(void)
{
    static unsigned char previous[MAX_NUMBER_OF_INPUT_KEYS];
    static unsigned int blocked, previous_buttons;
    static int trigger_left, trigger_right;
    static int previous_menu = -1;
    unsigned char next[MAX_NUMBER_OF_INPUT_KEYS] = {0};
    OrbisPadData data = {0};
    int menu = main_loop_state != 5 || InGameMenusAreRunning();
    ps4_pad_gameplay = 0;
    ps4_move_x = ps4_move_y = ps4_look_x = ps4_look_y = 0;
    /* SDL's PS4 instance ID is the scePad handle. Read directly so cached SDL
     * axes/buttons cannot stay held after a disconnect. */
    if (joy && SDL_JoystickGetAttached(joy) &&
        scePadReadState(SDL_JoystickInstanceID(joy), &data) == 0 && data.connected) {
        unsigned int buttons = data.buttons;
        /* Some pad providers expose triggers only through analogButtons.
         * Separate press/release thresholds prevent chatter near the edge. */
        trigger_left = data.analogButtons.l2 > (trigger_left ? 24 : 40);
        trigger_right = data.analogButtons.r2 > (trigger_right ? 24 : 40);
        if (trigger_left) buttons |= ORBIS_PAD_BUTTON_L2;
        if (trigger_right) buttons |= ORBIS_PAD_BUTTON_R2;
        /* Require release across menu/game transitions: no confirm-to-jump. */
        if (previous_menu != -1 && previous_menu != menu)
            blocked |= buttons & previous_buttons;
        blocked &= buttons;
        previous_buttons = buttons;
        buttons &= ~blocked;
#define PAD(name) ((buttons & ORBIS_PAD_BUTTON_##name) != 0)
#define SET(k,down) do { unsigned key=(k); if(key<MAX_NUMBER_OF_INPUT_KEYS && key!=KEY_VOID && (down)) next[key]=1; } while(0)
        SET(KEY_ESCAPE, PAD(OPTIONS) || (menu && PAD(CIRCLE)));
        if (menu) {
            SET(KEY_UP, PAD(UP) || data.leftStick.y < 74);
            SET(KEY_DOWN, PAD(DOWN) || data.leftStick.y > 182);
            SET(KEY_LEFT, PAD(LEFT) || data.leftStick.x < 74);
            SET(KEY_RIGHT, PAD(RIGHT) || data.leftStick.x > 182);
            SET(KEY_CR, PAD(CROSS));
        } else {
            PLAYER_INPUT_CONFIGURATION *p = AvP.PlayerType == I_Alien ? &AlienInputPrimaryConfig :
                AvP.PlayerType == I_Predator ? &PredatorInputPrimaryConfig : &MarineInputPrimaryConfig;
            ps4_pad_gameplay = 1;
            avp_stick(data.leftStick.x, data.leftStick.y, .15f, 0, &ps4_move_x, &ps4_move_y);
            avp_stick(data.rightStick.x, data.rightStick.y, .12f, 1, &ps4_look_x, &ps4_look_y);
            SET(p->Jump, PAD(CROSS) || (AvP.PlayerType == I_Alien && PAD(L1)));
            SET(p->Crouch, PAD(CIRCLE) || PAD(R3) || (AvP.PlayerType == I_Alien && PAD(R1)));
            SET(p->Operate, PAD(SQUARE));
            SET(p->FirePrimaryWeapon, PAD(R2));
            SET(p->FireSecondaryWeapon, PAD(L2));
            SET(p->Walk, PAD(L3)); /* Original game runs by default. */
            if (PAD(L3)) { ps4_look_x *= .5f; ps4_look_y *= .5f; }
            if (AvP.PlayerType == I_Alien) {
                SET(p->a.AlternateVision, PAD(TRIANGLE) || PAD(UP));
                SET(p->b.Taunt, PAD(DOWN));
            } else {
                SET(p->a.NextWeapon, PAD(R1));
                SET(p->b.PreviousWeapon, PAD(L1));
                if (AvP.PlayerType == I_Marine) {
                    SET(p->c.FlashbackWeapon, PAD(TRIANGLE));
                    SET(p->d.ImageIntensifier, PAD(UP));
                    SET(p->e.ThrowFlare, PAD(DOWN));
                    SET(p->a.NextWeapon, PAD(RIGHT));
                    SET(p->b.PreviousWeapon, PAD(LEFT));
                } else {
                    SET(p->d.Cloak, PAD(TRIANGLE));
                    SET(p->e.CycleVisionMode, PAD(UP));
                    SET(p->f.ZoomIn, PAD(RIGHT));
                    SET(p->g.ZoomOut, PAD(LEFT));
                    SET(p->i.RecallDisc, PAD(DOWN));
                }
            }
        }
#undef SET
#undef PAD
    } else {
        blocked = previous_buttons = 0;
        trigger_left = trigger_right = 0;
    }
    previous_menu = menu;
    for (unsigned k=0; k<MAX_NUMBER_OF_INPUT_KEYS; ++k) {
        /* Edge-only calls avoid repeating Enter into profile text fields. */
        if(next[k] != previous[k]) handle_keypress(k, 0, next[k]);
        else if (next[k]) KeyboardInput[k] = 1; /* mouse polling may clear it */
    }
    memcpy(previous, next, sizeof(previous));
}

/* Legacy mouse/joystick processing is disabled on PS4. Feed fixed-point rates;
 * pmove applies NormalFrameTime, so the curve does not depend on frame rate. */
void ps4_apply_pad(PLAYER_STATUS *p)
{
    if (!ps4_pad_gameplay || !IOFOCUS_AcceptControls() || InGameMenusAreRunning()) return;
    p->Mvt_MotionIncrement = (int)(-ps4_move_y * ONE_FIXED);
    p->Mvt_SideStepIncrement = (int)(ps4_move_x * ONE_FIXED);
    p->Mvt_TurnIncrement = (int)(ps4_look_x * ONE_FIXED * 1.35f);
    p->Mvt_PitchIncrement = (int)(ps4_look_y * ONE_FIXED * 1.5f);
    if (ControlMethods.FlipVerticalAxis) p->Mvt_PitchIncrement = -p->Mvt_PitchIncrement;
    p->Mvt_InputRequests.Flags.Rqst_Forward = p->Mvt_MotionIncrement > 0;
    p->Mvt_InputRequests.Flags.Rqst_Backward = p->Mvt_MotionIncrement < 0;
    p->Mvt_InputRequests.Flags.Rqst_SideStepLeft = p->Mvt_SideStepIncrement < 0;
    p->Mvt_InputRequests.Flags.Rqst_SideStepRight = p->Mvt_SideStepIncrement > 0;
    p->Mvt_InputRequests.Flags.Rqst_TurnLeft = p->Mvt_TurnIncrement < 0;
    p->Mvt_InputRequests.Flags.Rqst_TurnRight = p->Mvt_TurnIncrement > 0;
    p->Mvt_InputRequests.Flags.Rqst_LookUp = p->Mvt_PitchIncrement < 0;
    p->Mvt_InputRequests.Flags.Rqst_LookDown = p->Mvt_PitchIncrement > 0;
    p->Mvt_AnalogueTurning = p->Mvt_AnaloguePitching = 1;
    ControlMethods.AutoCentreOnMovement = 0;
}
