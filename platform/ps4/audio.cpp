#include <AL/alext.h>
#include <cstdlib>
#include <cstdio>
#include "audio.h"
extern "C" ALCdevice *avp_ps4_open_audio(void) { return ps4::openAudioDevice(); }
extern "C" ALCcontext *avp_ps4_audio_context(ALCdevice *device) {
    const ALCint attributes[] = {ALC_FREQUENCY,48000,
        ALC_FORMAT_CHANNELS_SOFT,ALC_STEREO_SOFT,ALC_FORMAT_TYPE_SOFT,ALC_FLOAT_SOFT,0};
    return alcCreateContext(device, attributes);
}
extern "C" int avp_ps4_start_audio(ALCdevice *device) {
    if (!ps4::startAudioOutput(device)) return 0;
    static bool registered=false;
    if (!registered) { std::atexit(ps4::stopAudioOutput); registered=true; }
    return 1;
}
extern "C" void avp_ps4_stop_audio(void) { ps4::stopAudioOutput(); }
