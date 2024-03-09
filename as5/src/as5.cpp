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
    raylib::Sound ping ("audio/ping.wav");
    raylib::Music crowd ("audio/crowd.wav");
    raylib::Music pof ("audio/price-of-freedom.mp3");

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
    GuiVolumeControlState guiState = InitGuiVolumeControl();

    window.SetTargetFPS(60);
    while (!window.ShouldClose()) {
        inputs.PollEvents();

        window.BeginDrawing();
        {
			ClearBackground(raylib::Color{34, 40, 49, 255});

            //Updates the volume of the different sound
            GuiVolumeControl(&guiState);
            
            //Make sure the audio has enough buffer
            pof.Update();
            crowd.Update();

            //Plays the music 
            pof.Play();
            crowd.Play();

            // Set volume after updating sounds
            ping.SetVolume(guiState.SFXSliderValue / 100);
            crowd.SetVolume(guiState.DialogueSliderValue / 100);
            pof.SetVolume(guiState.MusicSliderValue / 100);
        }
        window.EndDrawing();
    }

    CloseAudioDevice();
    return 0;
}

