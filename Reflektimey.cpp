#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <FL/fl_draw.H>
#include <random>
#include <chrono>
#include <thread>
#include <string>
#include <sstream>
#include <stdexcept> // For error handling

// Global variables
float delay = -1.0f; // Holds the randomized delay value
bool countdown_in_progress = false;
bool allow_start = false;
std::chrono::high_resolution_clock::time_point start_time;

// Utility function to get a random float between min and max
float getRandomFloat(float min, float max) {
    try {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(min, max);
        return dis(gen);
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Error generating random float: " + std::string(e.what()));
    }
}

// Function to create a popup window
void showPopup(const std::string& message, int duration_ms) {
    try {
        auto* popup = new Fl_Window(300, 100, "Pesan");
        Fl_Box* box = new Fl_Box(0, 0, 300, 100, message.c_str());
        box->box(FL_FLAT_BOX);
        box->labelsize(16);
        box->labelfont(FL_BOLD);
        box->align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
        popup->end();
        popup->show();

        // Close the popup after a delay
        std::this_thread::sleep_for(std::chrono::milliseconds(duration_ms));
        popup->hide();
        delete popup;
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Error showing popup: " + std::string(e.what()));
    }
}

// "Acak" button callback
void randomizeDelay(Fl_Widget* w, void*) {
    try {
        delay = getRandomFloat(0.0f, 5.0f);
        allow_start = true; // Enable "Mulai" button
        std::ostringstream oss;
        oss << "Waktu diacak: " << delay << " detik";
        showPopup(oss.str(), 2000);
    }
    catch (const std::exception& e) {
        showPopup("Error: " + std::string(e.what()), 2000);
    }
}

// "Mulai/Berhenti" button callback
void startStopCallback(Fl_Widget* w, void* window) {
    try {
        Fl_Button* button = (Fl_Button*)w;
        Fl_Window* main_window = (Fl_Window*)window;

        if (button->label() == std::string("Mulai") && allow_start) {
            countdown_in_progress = true;
            button->label("Berhenti");
            main_window->color(FL_WHITE);
            main_window->redraw();

            // Start countdown
            start_time = std::chrono::high_resolution_clock::now();
            std::this_thread::sleep_for(std::chrono::duration<float>(delay));
            countdown_in_progress = false;

            // Update UI after countdown
            main_window->color(FL_GREEN);
            main_window->redraw();

        }
        else if (button->label() == std::string("Berhenti")) {
            if (countdown_in_progress) {
                // Countdown interrupted
                static const std::string messages[] = {
                    "Lo gak sabaran apa gimana?",
                    "Apasi masalah di hidup lo?",
                    "Santai bro, santai",
                    "Sambil merem ya mata lo?"
                };
                int random_index = static_cast<int>(getRandomFloat(0, 4));
                showPopup(messages[random_index], 2000);
            }
            else {
                // Countdown finished
                auto end_time = std::chrono::high_resolution_clock::now();
                float elapsed = std::chrono::duration<float>(end_time - start_time).count();

                if (std::abs(elapsed - delay) < 0.001f) {
                    showPopup("Tamat! Selamat! Sehat?", 2000);
                }
                else {
                    std::ostringstream oss;
                    oss << "Biasa aja, waktu reflek: " << elapsed - delay << " detik";
                    showPopup(oss.str(), 2000);
                }
            }

            // Reset button
            button->label("Mulai");
            main_window->color(FL_BACKGROUND_COLOR);
            main_window->redraw();
            allow_start = false;
        }
    }
    catch (const std::exception& e) {
        showPopup("Error: " + std::string(e.what()), 2000);
    }
}

int main() {
    try {
        Fl_Window* window = new Fl_Window(400, 300, "Aplikasi Timer FLTK");

        // "Mulai/Berhenti" button
        Fl_Button* start_button = new Fl_Button(160, 120, 80, 40, "Mulai");
        start_button->callback(startStopCallback, window);

        // "Acak" button
        Fl_Button* randomize_button = new Fl_Button(300, 250, 80, 40, "Acak");
        randomize_button->callback(randomizeDelay);

        window->end();
        window->show();

        return Fl::run();
    }
    catch (const std::exception& e) {
        showPopup("Fatal Error: " + std::string(e.what()), 2000);
        return 1;
    }
}
