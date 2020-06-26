#pragma once

// Technically, DTS may contain 256-long blocks:
// For normal frames, this indicates a window size of either 4096, 2048, 1024, 512, or 256 samples per channel.

// However, 256 is too small for ALSA period on Pi4.
// If some real-world media files indeed use 256 samples, will fix eventually, need to initialize ALSA period equal to 2 samples as opposed to just 1.
// For now, using 512-long blocks. As a nice side effect they work slightly faster, 2x less function calls, better CPU cache utilization, etc.
constexpr int samplesPerBlock = 0x200;