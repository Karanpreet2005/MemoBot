#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h> // now not using EEPROM

// Motor A
int IN1 = D5; // Replace with your actual pin number
int IN2 = D6; // Replace with your actual pin number

// Motor B
int IN3 = D7; // Replace with your actual pin number
int IN4 = D8; // Replace with your actual pin number

WiFiServer server(80);
ESP8266WebServer webServer(80);

// Define the maximum number of commands to store
const int maxCommands = 100;
char commandArray[maxCommands];
int commandIndex = 0;

void setup()
{
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin("Karanpreet", "kpreet2k5");
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());

  // Setup routes for the web server
  webServer.on("/", HTTP_GET, handleRoot);
  webServer.on("/F", HTTP_GET, handleForward);
  webServer.on("/B", HTTP_GET, handleBackward);
  webServer.on("/L", HTTP_GET, handleLeft);
  webServer.on("/R", HTTP_GET, handleRight);
  //webServer.on("/S", HTTP_GET, handleStore);
 // webServer.on("/P", HTTP_GET, handlePrintEEPROM);
  webServer.on("/P", HTTP_GET, handlePrintArray);
  webServer.on("/C", HTTP_GET, handleClear);
  webServer.on("/X", HTTP_GET, handleStop);
  webServer.on("/T", HTTP_GET, handleRetrace);
  webServer.on("/W", HTTP_GET, handleRetrace2);
  // Start the web server
  webServer.begin();
}

void loop()
{
  webServer.handleClient();
}

void handleForward() 
{
  moveForward();
  addToPathMemory('F');//this stores 'F' in addToPathMemory
  sendResponse("Moving forward");
}

void handleBackward() 
{
  moveBackward();
  addToPathMemory('B');
  sendResponse("Moving backward");
}

void handleRight() 
{
  turnRight();
  addToPathMemory('R');
  sendResponse("Turning right");
}

void handleLeft() 
{
  turnLeft();
  addToPathMemory('L');
  sendResponse("Turning left");
}

//void handleStore() 
//{
  //storePathToEEPROM();
  //sendResponse("Path stored");
//}

void handleClear() 
{
  clearPathMemory();
  sendResponse("Memory cleared");
}

void handleStop() 
{
  stopMotors();
  sendResponse("Motors stopped");
}

void handleRetrace() 
{
  retracePath();
  sendResponse("Retracing path");
}
void handlePrintArray()
{
  printArrayContents();
  sendResponse("Array contents printed");
}
void handleRetrace2()
{
  retracePath2();
  sendResponse("Retracing path2");
}
void handleRoot() 
{
  String html = "<html><body>";
  html += "<h1>Hello from your robot!</h1>";
  html += "<hr>";
  html += "<marquee>G-23</marquee>";
  html += "<br>";
  html += "<br>";
  html += "<hr>";
  html += "<hr>";
  html += "<button onclick=\"sendCommand('F')\">Forward</button>";
  html += "<button onclick=\"sendCommand('B')\">Backward</button>";
  html += "<button onclick=\"sendCommand('L')\">Right</button>";
  html += "<button onclick=\"sendCommand('R')\">Left</button>";
  html += "<hr>";
  html += "<hr>";
  html += "<br>";
  html += "<br>";
//  html += "<button onclick=\"sendCommand('S')\">Store Path</button>";
//  html += "<button onclick=\"sendCommand('P')\">Print EEPROM Contents</button>";
  html += "<button onclick=\"sendCommand('P')\">Print Array Contents</button>";

  html += "<button onclick=\"sendCommand('C')\">Clear Memory</button>";
  html += "<button onclick=\"sendCommand('X')\">Stop Motors</button>";
  html += "<button onclick=\"sendCommand('T')\">Retrace Path</button>";
  html += "<button onclick=\"sendCommand('W')\">Retrace Path2</button>";
  html += "<script>function sendCommand(command) {";
  html += "var xhr = new XMLHttpRequest(); xhr.open('GET', '/' + command, true); xhr.send();";
  html += "}</script>";
  html += "</body></html>";

  webServer.send(200, "text/html", html);
}

void sendResponse(String message) 
{
  webServer.send(200, "text/plain", message);
}

void moveForward() 
{
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  delay(800);//adjust according to your requirement
  stopMotors();
}

void moveBackward() 
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  delay(800);//adjust according to your requirement
  stopMotors();
}

void turnRight() 
{
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  delay(800);//adjust according to your requirement
  stopMotors();
  }

void turnLeft() 
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  delay(800);//adjust according to your requirement
  stopMotors();
  }

void stopMotors() 
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

// Update the addToPathMemory function
void addToPathMemory(char command) {
  if (commandIndex < maxCommands) {
    commandArray[commandIndex] = command;
    commandIndex++;
  }
}

//void storePathToEEPROM() //store path should store the commands passed and not the store path command , eg: F F B B L R
//{
  // Reset EEPROM address to start storing from the beginning
 // eepromAddress = 0;

  // Store each command in EEPROM
  //addToPathMemory('S');  // Mark the start of the path
  // Add more commands as needed

  //EEPROM.end();  // Release EEPROM
//}

void clearPathMemory() {
  memset(commandArray, 0, sizeof(commandArray));
  commandIndex = 0;
}

void retracePath() {
  rotate180Degrees();
  delay(1000); //adjust according to your requirement
  for (int i = commandIndex - 1; i >= 0; i--) {
    char command = commandArray[i];
    Serial.println(command);

    // Execute the reverse of each command
    switch (command) {
      case 'F':
        moveForward();
        break;
      case 'B':
        moveBackward();
        break;
      case 'L':
        turnRight();  // Reverse direction for left turn
        break;
      case 'R':
        turnLeft();   // Reverse direction for right turn
        break;
      // Add more cases as needed
    }

    

    // Delay to give the robot time to perform the action
    delay(1000);
  }

  // Stop the motors after retracing
  stopMotors();
  sendResponse("Path retraced");
}
void retracePath2()// here rotating without 180 degrees turn
 {
  for (int i = commandIndex - 1; i >= 0; i--) //change the commandIndex-1 to commandIndex if last command is not executed 
   {
    char command = commandArray[i];
    Serial.println(command);

    // Execute the reverse of each command
    switch (command) {
      case 'F':
        moveBackward();
        break;
      case 'B':
        moveForward();
        break;
      case 'L':
        turnRight();  // Reverse direction for left turn
        break;
      case 'R':
        turnLeft();   // Reverse direction for right turn
        break;
      // Add more cases as needed
    }

    

    // Delay to give the robot time to perform the action
    delay(1000);
  }

  // Stop the motors after retracing
  stopMotors();
  sendResponse("Path retraced2");
}

void rotate180Degrees() 
{
  // Implement the logic to rotate the robot 180 degrees
  // This might involve using motor commands to turn the robot
  // Make sure to adjust the duration and speed based on your robot's characteristics
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  delay(1300);  // Adjust the delay based on the time it takes to rotate 180 degrees
  stopMotors();

}
void printArrayContents()
{

Serial.print("Stored Commands: ");
  for (int i = 0; i < commandIndex; i++) {
    Serial.print(commandArray[i]);
    Serial.print(" ");
  }
  Serial.println(); // Print a newline at the end
}