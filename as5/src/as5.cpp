#include <raylib-cpp.hpp>
#include <BufferedInput.hpp>
#include "delegate.hpp"
#include "VolumeControl.h"

cs381::Delegate<void()> PingButton;

#define GUI_VOLUMECONTROL_IMPLEMENTATION
#include "VolumeControl.h"

int main(){
    //Set up sound
    InitAudioDevice();
    raylib::Sound crowd("audio/crowd.wav");
    raylib::Sound ping("audio/ping.wav");
    raylib::Music pof("audio/price-of-freedom.mp3");

    //Init ping button
    PingButton +=[&ping]() {
        ping.Play();
    };

    //Set up the window
    int winWidth = 300;
    int winHeight = 350;
    raylib::Window window(winWidth, winHeight, "CS381 - Assignment 5");

    //Set up buffered input
    raylib::BufferedInput inputs;
    inputs["ping"] = raylib::Action::key(KEY_SPACE).SetPressedCallback([]{
        PingButton();
    }).move();

    //Set up GUI
    auto guiState = InitGuiVolumeControl();

    window.SetTargetFPS(60);
    while (!window.ShouldClose()) {
        inputs.PollEvents();

        window.BeginDrawing();
        {
			ClearBackground(RAYWHITE);

            GuiVolumeControl(&guiState);

        }
        window.EndDrawing();
    }
    return 0;
}

