/*______ _       ___   _   _ _____   _   _ _____ ___________ _____  ___  
| ___ \ |     / _ \ | \ | |_   _| | | | |_   _|  _  | ___ \_   _|/ _ \ 
| |_/ / |    / /_\ \|  \| | | |   | | | | | | | | | | |_/ / | | / /_\ \
|  __/| |    |  _  || . ` | | |   | | | | | | | | | |  __/  | | |  _  |
| |   | |____| | | || |\  | | |   | |_| | | | \ \_/ / |    _| |_| | | |
\_|   \_____/\_| |_/\_| \_/ \_/    \___/  \_/  \___/\_|    \___/\_| |_/
Github: https://github.com/Jord-J/Plant-Utopia             Version: 2.0 
*/

// -- NOTE: Bluetooth module only requires prints to Serial, we plug RX pin on bluetooth module to TX on arduino so it mimics the arduino
// -- NOTE: In order to run this code you need to include the library for DHT which is also included on the github repository
// Below are the applications we used for showcasing purposes for the bluetooth
// IOS: https://apps.apple.com/gb/app/blexar/id1439459314
// Google Play:

// Includes
#include <dht.h> // The library for this can be found on the github repository https://github.com/Jord-J/Plant-Utopia

// Defines - these are the pin allocations for the sensors/water pump
#define temperature_sensor A0
#define soil_sensor A1
#define water_level_bc A2
#define water_pump_bc 7
#define water_level_sc A4
#define water_pump_sc 8

// Testing defines *** ONLY CHANGE THESE PARAMETERS ***
#define codePause 10 // Delay between each code loop (seconds)
#define secondsForWaterpumpActive 5 // The time the water pump is active (seconds)

// Initialisers
dht DHT;
float humidity = 0;
float temperature = 0;
int moisture = 0;
int waterLevelbc = 0;
int waterLevelsc = 0;
static int interval = codePause; // static as this value never changes
static int timeForPump = secondsForWaterpumpActive; // static as this value never changes
int water_level_sc_val = 0;

// Function gets the user input from the Serial monitor
char* serialString()
{
  static char str[4]; // String array only allows 3 characters which is what we only require as our command is 'run'
  if(!Serial.available()) return NULL; // Checks to see if there is any bytes available, if not then return null (move onto next iteration of loop)
  delay(64); // Wait for all characters to arrive
  memset(str,0,sizeof(str)); // Set block of memory for str to 0
  byte count=0;
  while(Serial.available())
  {
    char c=Serial.read(); // Reads the first byte from the buffer and removes that byte from the buffer
    if (c>=32 && count<sizeof(str)-1)
    {
      str[count]=c; // Assign the byte to String based on count
      count++;
    }
  }
  str[count]='\0'; // \0 is known as null character, acts as a string terminator (standard C strings are null-terminated)
  return str;
}

// Begin serial to access serial monitor & set pin modes for water pumps to OUTPUT
void setup() 
{
  Serial.begin(9600);
  delay(1500); // Initial delay on running code
  pinMode(water_pump_bc, OUTPUT); // Set pinMode to OUTPUT as we want to provide power to the water pump when required in our loop
  pinMode(water_pump_sc, OUTPUT); // Set pinMode to OUTPUT as we want to provide power to the water pump when required in our loop
}

void loop() 
{
  delay(interval*1000);
  Serial.println("\n\n"); // Delay between each code loop
  waterLevelbc = analogRead(water_level_bc);
  waterLevelsc = analogRead(water_level_sc);
  moisture = analogRead(soil_sensor);
  moisture = map(moisture, 0, 1023, 0, 100); // Get value of moisture as a value between 0 - 100 to resemble percentage
  DHT.read11(temperature_sensor);
  humidity = DHT.humidity;
  temperature = DHT.temperature;
  Serial.println("Plant environment data:\n\n");
  delay(1000);
  Serial.print("Humidity sensor value = ");
  Serial.print(humidity);
  Serial.println("%");
  delay(1000);
  Serial.print("Temperature sensor value = ");
  Serial.print(temperature);
  Serial.println("°C");
  delay(1000);
  Serial.print("Big container Water level value  = ");
  Serial.println(waterLevelbc);
  delay(1000);
  Serial.print("Small container Water level value  = ");
  Serial.println(waterLevelsc);
  delay(1000);
  Serial.print("Moisture sensor value = ");
  Serial.print(moisture);
  Serial.println("\n\n");
  delay(1000);
  char* userInput = serialString();
  // We check to see if user has typed 'run', if they have & there is enough water, run water pump code
  if(userInput != NULL && String(userInput) == "run" && waterLevelbc >= 640)
  {
    Serial.println("\n\n");
    Serial.println("You have entered the run command for watering plant!");
    Serial.println("- - - - - - - - - - - - - - - - -");
    Serial.print("Now pumping water for ");
    Serial.print(timeForPump);
    Serial.println(" seconds");
    analogWrite(water_pump_bc, 255);
    delay(timeForPump*1000);
    digitalWrite(water_pump_bc, 0);
    Serial.println("Finished");
    delay(3000);
    water_level_sc_val = analogRead(water_level_sc);
    // Check to see if small container has too much water, if so, recycle the water back into main water storage
    if(water_level_sc_val >= 447) 
    {
      Serial.println("Restoring water to water container...");
      digitalWrite(water_pump_sc, 255);
      delay(35000);
      digitalWrite(water_pump_sc,0); 
    }
    else 
    {
      // This is for debugging purposes! Not really necessary to print
      Serial.println("Not enough water!! Fill the small container!!");
    }
  }
  else if(waterLevelbc < 640)
  {
    // We know that there is not enough water to so we notify the user to add more water
    Serial.println("Not enough water, please add more water!\n\n");
  }
}
