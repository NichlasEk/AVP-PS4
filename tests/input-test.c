/* Host tests exercise the actual adapter with a mocked pad/engine boundary. */
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <orbis/_types/pad.h>
#define AVP_PAD_TEST 1
#define MAX_NUMBER_OF_INPUT_KEYS 256
#define ONE_FIXED 65536
enum { KEY_VOID=255, KEY_ESCAPE=1, KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_CR };
enum { I_Marine, I_Alien, I_Predator };
typedef struct {
    unsigned char Jump, Crouch, Operate, FirePrimaryWeapon, FireSecondaryWeapon, Walk;
    union { unsigned char NextWeapon, AlternateVision; } a;
    union { unsigned char PreviousWeapon, Taunt; } b;
    struct { unsigned char FlashbackWeapon; } c;
    union { unsigned char Cloak, ImageIntensifier; } d;
    union { unsigned char CycleVisionMode, ThrowFlare; } e;
    struct { unsigned char ZoomIn; } f;
    struct { unsigned char ZoomOut; } g;
    struct { unsigned char RecallDisc; } i;
} PLAYER_INPUT_CONFIGURATION;
static PLAYER_INPUT_CONFIGURATION MarineInputPrimaryConfig, AlienInputPrimaryConfig, PredatorInputPrimaryConfig;
typedef struct {
    int Mvt_MotionIncrement, Mvt_SideStepIncrement, Mvt_TurnIncrement, Mvt_PitchIncrement;
    int Mvt_AnalogueTurning, Mvt_AnaloguePitching;
    struct { struct {
        int Rqst_Forward, Rqst_Backward, Rqst_SideStepLeft, Rqst_SideStepRight;
        int Rqst_TurnLeft, Rqst_TurnRight, Rqst_LookUp, Rqst_LookDown;
    } Flags; } Mvt_InputRequests;
} PLAYER_STATUS;
static struct { int PlayerType; } AvP;
static struct { int FlipVerticalAxis, AutoCentreOnMovement; } ControlMethods;
static int main_loop_state=1, menu_active, focus=1, connected=1, read_error;
static void *joy=(void*)1;
static OrbisPadData snapshot;
static unsigned char KeyboardInput[256];
static int presses[256];
static int InGameMenusAreRunning(void) { return menu_active; }
static int IOFOCUS_AcceptControls(void) { return focus; }
static int SDL_JoystickGetAttached(void *p) { return p!=NULL; }
static int SDL_JoystickInstanceID(void *p) { (void)p; return 42; }
static int scePadReadState(int handle, OrbisPadData *out) {
    assert(handle==42); *out=snapshot; out->connected=connected; return read_error;
}
static void handle_keypress(int k, int unicode, int down) {
    (void)unicode; KeyboardInput[k]=down; if(down) presses[k]++;
}
#include "input.inc"
static void neutral(void) {
    snapshot.buttons=0;
    snapshot.analogButtons.l2=snapshot.analogButtons.r2=0;
    snapshot.leftStick.x=snapshot.leftStick.y=128;
    snapshot.rightStick.x=snapshot.rightStick.y=128;
    ps4_poll_pad();
}
static void button(unsigned mask, unsigned key) {
    neutral(); snapshot.buttons=mask; ps4_poll_pad(); assert(KeyboardInput[key]);
    for (unsigned k=0;k<256;k++) assert(!KeyboardInput[k] || k==key);
    int before=presses[key];
    /* Simulate main2's mouse poll clearing the primary-fire key every frame. */
    KeyboardInput[key]=0; ps4_poll_pad();
    assert(KeyboardInput[key] && presses[key]==before);
    neutral(); assert(!KeyboardInput[key]);
}
int main(void) {
    float x,y,prev=0;
    for(int a=0;a<256;a++) for(int b=0;b<256;b++) {
        avp_stick(a,b,.15f,0,&x,&y);
        assert(isfinite(x) && isfinite(y) && x*x+y*y<=1.00001f);
        avp_stick(a,b,.12f,1,&x,&y);
        assert(isfinite(x) && isfinite(y) && x*x+y*y<=1.00001f);
    }
    for(int a=128;a<256;a++) {
        avp_stick(a,128,.12f,1,&x,&y); assert(x>=prev && y==0); prev=x;
    }
    avp_stick(128,128,.12f,1,&x,&y); assert(x==0 && y==0);
    avp_stick(140,125,.12f,1,&x,&y); assert(x==0 && y==0);
    avp_stick(255,128,.12f,1,&x,&y); assert(fabsf(x-1)<.00001f);
    avp_stick(0,128,.12f,1,&x,&y); assert(fabsf(x+1)<.00001f);
    PLAYER_INPUT_CONFIGURATION p={20,21,22,23,24,25,{26},{27},{28},{29},{30},{31},{32},{33}};
    MarineInputPrimaryConfig=AlienInputPrimaryConfig=PredatorInputPrimaryConfig=p;
    neutral(); button(ORBIS_PAD_BUTTON_CROSS,KEY_CR);
    button(ORBIS_PAD_BUTTON_CIRCLE,KEY_ESCAPE);
    button(ORBIS_PAD_BUTTON_UP,KEY_UP);
    /* Holding confirmation across a transition must not trigger jump. */
    snapshot.buttons=ORBIS_PAD_BUTTON_CROSS; ps4_poll_pad();
    main_loop_state=5; ps4_poll_pad(); assert(!KeyboardInput[p.Jump]); neutral();
    for(int species=0;species<3;species++) {
        AvP.PlayerType=species;
        button(ORBIS_PAD_BUTTON_CROSS,p.Jump); button(ORBIS_PAD_BUTTON_CIRCLE,p.Crouch);
        button(ORBIS_PAD_BUTTON_R3,p.Crouch); button(ORBIS_PAD_BUTTON_SQUARE,p.Operate);
        button(ORBIS_PAD_BUTTON_R2,p.FirePrimaryWeapon); button(ORBIS_PAD_BUTTON_L2,p.FireSecondaryWeapon);
        button(ORBIS_PAD_BUTTON_L3,p.Walk); button(ORBIS_PAD_BUTTON_OPTIONS,KEY_ESCAPE);
        if(species==I_Alien) {
            button(ORBIS_PAD_BUTTON_L1,p.Jump); button(ORBIS_PAD_BUTTON_R1,p.Crouch);
            button(ORBIS_PAD_BUTTON_TRIANGLE,p.a.AlternateVision);
            button(ORBIS_PAD_BUTTON_UP,p.a.AlternateVision); button(ORBIS_PAD_BUTTON_DOWN,p.b.Taunt);
        } else {
            button(ORBIS_PAD_BUTTON_R1,p.a.NextWeapon); button(ORBIS_PAD_BUTTON_L1,p.b.PreviousWeapon);
            if(species==I_Marine) {
                button(ORBIS_PAD_BUTTON_TRIANGLE,p.c.FlashbackWeapon);
                button(ORBIS_PAD_BUTTON_UP,p.d.ImageIntensifier); button(ORBIS_PAD_BUTTON_DOWN,p.e.ThrowFlare);
                button(ORBIS_PAD_BUTTON_LEFT,p.b.PreviousWeapon); button(ORBIS_PAD_BUTTON_RIGHT,p.a.NextWeapon);
            } else {
                button(ORBIS_PAD_BUTTON_TRIANGLE,p.d.Cloak); button(ORBIS_PAD_BUTTON_UP,p.e.CycleVisionMode);
                button(ORBIS_PAD_BUTTON_DOWN,p.i.RecallDisc); button(ORBIS_PAD_BUTTON_LEFT,p.g.ZoomOut);
                button(ORBIS_PAD_BUTTON_RIGHT,p.f.ZoomIn);
            }
        }
    }
    neutral(); snapshot.analogButtons.r2=100; ps4_poll_pad();
    assert(KeyboardInput[p.FirePrimaryWeapon]);
    snapshot.analogButtons.r2=30; ps4_poll_pad(); assert(KeyboardInput[p.FirePrimaryWeapon]);
    snapshot.analogButtons.r2=20; ps4_poll_pad(); assert(!KeyboardInput[p.FirePrimaryWeapon]);
    snapshot.analogButtons.r2=30; ps4_poll_pad(); assert(!KeyboardInput[p.FirePrimaryWeapon]);
    neutral(); snapshot.analogButtons.l2=100; ps4_poll_pad(); assert(KeyboardInput[p.FireSecondaryWeapon]);
    neutral();
    PLAYER_STATUS state={0};
    snapshot.leftStick.y=0; snapshot.rightStick.x=255; snapshot.rightStick.y=0;
    ps4_poll_pad(); ps4_apply_pad(&state);
    assert(state.Mvt_MotionIncrement==ONE_FIXED && state.Mvt_SideStepIncrement==0);
    assert(state.Mvt_TurnIncrement>0 && state.Mvt_PitchIncrement<0);
    int yaw=state.Mvt_TurnIncrement;
    snapshot.buttons=ORBIS_PAD_BUTTON_L3; ps4_poll_pad(); ps4_apply_pad(&state);
    assert(abs(state.Mvt_TurnIncrement*2-yaw)<=1);
    ControlMethods.FlipVerticalAxis=1; ps4_apply_pad(&state); assert(state.Mvt_PitchIncrement>0);
    neutral(); ps4_apply_pad(&state);
    assert(!state.Mvt_MotionIncrement && !state.Mvt_TurnIncrement && !state.Mvt_PitchIncrement);
    snapshot.buttons=ORBIS_PAD_BUTTON_R2; ps4_poll_pad(); assert(KeyboardInput[p.FirePrimaryWeapon]);
    connected=0; ps4_poll_pad(); assert(!KeyboardInput[p.FirePrimaryWeapon] && !ps4_pad_gameplay);
    connected=1; read_error=-1; ps4_poll_pad(); assert(!ps4_pad_gameplay);
    read_error=0; neutral(); focus=0; snapshot.leftStick.y=0;
    ps4_poll_pad(); memset(&state,0,sizeof(state)); ps4_apply_pad(&state); assert(!state.Mvt_MotionIncrement);
    focus=1; menu_active=1; ps4_poll_pad(); ps4_apply_pad(&state); assert(!state.Mvt_MotionIncrement);
    puts("PASS: 131072 stick samples, species mappings, held fire, transition guard, neutral, disconnect, focus, inversion and precision mode");
}
