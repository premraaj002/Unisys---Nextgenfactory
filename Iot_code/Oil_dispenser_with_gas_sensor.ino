
const int trigPin = 10;
const int echoPin = 11;


const int relay2 = 8;  

const int distanceThreshold = 10;  

void setup() {
  Serial.begin(9600);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  
  
  digitalWrite(relay1, LOW); 
  
 
  digitalWrite(relay2, HIGH); 
}

void loop() {
  // Measure distance from Ultrasonic Sensor
  long duration, distance;
  
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;  // Convert to cm
  
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  
  // Check if object is within threshold distance
  if (distance <= distanceThreshold && distance > 0) {
    digitalWrite(relay2, LOW);  
    Serial.println("Object detected! Relay 2 ON");
  } else {
    digitalWrite(relay2, HIGH); 
  }
  
  delay(500); 
}
