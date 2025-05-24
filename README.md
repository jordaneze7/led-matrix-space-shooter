Imperator - Arduino Space Shooter Game
A retro-style space shooter game designed for Arduino with RGB LED matrix display. Navigate your ship, dodge incoming enemies, and rack up points in this classic arcade-inspired experience!
🎮 Game Features

Classic Space Shooter Gameplay: Move your ship vertically and horizontally while shooting at incoming enemies
Progressive Difficulty: Game speed increases every 20 points, making survival increasingly challenging
Lives System: Start with 5 lives, lose one when enemies reach you or collide with your ship
High Score Tracking: Persistent high score storage using EEPROM memory
Multi-Projectile System: Fire up to 5 bullets simultaneously
Enemy AI: Smart enemy spawning with randomized positions and timing
Bonus Lives: Earn extra lives every level milestone
Cheat Codes: Hidden debug features for testing (can be disabled)

🛠 Hardware Requirements
Essential Components

Arduino Board (Uno, Mega, or compatible)
64x32 RGB LED Matrix with appropriate driver
7 Push Buttons for game controls
Pull-up resistors for button inputs
Power supply capable of driving the LED matrix

Pin Configuration
cppButton Controls:
- BTN_UP (Pin 35): Move ship up
- BTN_DOWN (Pin 30): Move ship down  
- BTN_LEFT (Pin 36): Move ship left
- BTN_RIGHT (Pin 34): Move ship right
- BTN_A (Pin 31): Menu navigation / Cheat code
- BTN_B (Pin 33): Menu navigation / Cheat code
- BTN_C (Pin 32): Fire weapon
📚 Required Libraries
Install these libraries through the Arduino IDE Library Manager:
cpp#include <MOMO_RGB_Matrix.h>  // RGB Matrix control
#include <Bounce2.h>          // Button debouncing
#include <EEPROM.h>          // High score storage
🎯 How to Play
Game Controls

Arrow Buttons: Move your ship up, down, left, right
Button C: Fire projectiles at enemies
Buttons A/B: Navigate menus and restart game

Gameplay Mechanics

Objective: Survive as long as possible while destroying enemies
Scoring: +1 point per enemy destroyed
Lives: Start with 5 lives, gain 1 life every 20 points
Levels: Difficulty increases every 20 points
Game Over: When all lives are lost

Game States

Title Screen: Shows game name and high score
Active Game: Main gameplay with enemies, bullets, and interface
Game Over: Displays final score and level reached
Reset: Automatically returns to title screen

🔧 Installation & Setup

Hardware Assembly:

Connect RGB LED matrix to Arduino following MOMO_RGB_Matrix library documentation
Wire 7 push buttons to specified pins with pull-up resistors
Ensure adequate power supply for LED matrix


Software Installation:
bash# Clone this repository
git clone https://github.com/yourusername/imperator-arduino-game.git

# Install required libraries in Arduino IDE:
# - MOMO_RGB_Matrix
# - Bounce2
# - EEPROM (usually pre-installed)

Upload Code:

Open imperator.ino in Arduino IDE
Select your Arduino board and port
Compile and upload to your Arduino



⚙️ Configuration Options
Game Balance Settings
cpp#define MAX_BALLES 5              // Maximum simultaneous bullets
#define VITESSE_BALLES 15         // Bullet movement speed
#define VITESSE_ENNEMIES 50       // Enemy movement speed  
#define MAX_ENNEMIES 15           // Maximum enemies on screen
#define START_PLAYER_LIFE 5       // Starting lives
#define LEVEL_CHANGE 20           // Points needed per level
#define FACTEUR_AUGMENTATION 5    // Difficulty increase factor
Display Settings
cpp#define LARGEUR_MATRICE 64        // Matrix width
#define HAUTEUR_MATRICE 32        // Matrix height
#define HAUTEUR_INTERFACE 22      // UI area height
Cheat Codes
cpp#define CHEATS_ON 1               // Enable/disable cheat codes
// Cheat: Press A button 4 times for +10 lives
// Cheat: Press B button 4 times to reset high score
🎨 Game Graphics
The game uses custom bitmap graphics for:

Player Ship: 7x3 pixel purple spacecraft
Enemy Ships: 6x3 pixel orange/yellow enemies
Projectiles: Single red pixels
UI Elements: Score, lives, and level display

🏆 Scoring System

+1 Point: Each enemy destroyed
+1 Life: Every 20 points earned
Level Up: Every 20 points (increases enemy speed)
High Score: Automatically saved to EEPROM memory

🐛 Troubleshooting
Common Issues

Display Problems: Check matrix wiring and power supply
Button Issues: Verify pull-up resistors and pin connections
Game Too Fast/Slow: Adjust speed constants in configuration
Memory Issues: Ensure adequate RAM for large enemy arrays

Debug Features

Serial output available at 115200 baud rate
Cheat codes can be used for testing gameplay mechanics

🤝 Contributing
Contributions are welcome! Areas for improvement:

Additional enemy types and patterns
Sound effects integration
Different weapon types
Enhanced graphics and animations
Power-up system
Multiplayer support

📄 License
This project is open source. Feel free to modify and distribute according to your needs.
🎖️ Credits

Game concept inspired by classic arcade space shooters
Built with Arduino ecosystem and community libraries
Designed for educational and entertainment purposes


Enjoy defending the galaxy in Imperator! 🚀
