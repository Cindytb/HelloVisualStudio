#include "audio.h"

#include <chrono>
#include <thread>


int main() {
  initializePA(44100);
  std::this_thread::sleep_for(std::chrono::seconds(20));
  closePA();
}
