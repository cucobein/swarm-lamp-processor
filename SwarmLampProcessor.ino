//#define DEBUG
#define column_height 12
#define n_columns 1
#define bit_duration 20  //  1/38400 = 26uS

typedef struct {
  unsigned char x;
  unsigned char y;
  unsigned char z;
} tCard;

tCard cards[n_columns][column_height];

void setup() {
  #ifdef DEBUG
    Serial.begin(38400);
  #endif
  initStruct();
  initOutputs();
}

void loop() {
  #ifdef DEBUG
    Serial.println("----------------");
    Serial.println("---START LOOP---");
    Serial.println("----------------");
  #endif
  initStruct();
  refreshData();
  #ifdef DEBUG
    Serial.println("--------------");
    Serial.println("---END LOOP---");
    Serial.println("--------------");
  #endif
  delay(30);
}

// MARK: Init
void initStruct() {
  int x = 0;
  for (int i = 0; i < n_columns; i++) {
    for (int j = 0; j < column_height; j++) {
      cards[i][j].x = 120;
      cards[i][j].y = 120;
      cards[i][j].z = 120;
    }
  }
}

void initOutputs() {
  for (int pin = 22; pin < (22 + n_columns); pin++) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH);
  }
}

// MARK: Software UART - TX
void refreshData() {
  for(int i = 0; i < column_height; i++) {
    sendStartByte();
    unsigned char current_address = i + 1;
    unsigned char addresses[n_columns];
    unsigned char x_values[n_columns];
    unsigned char y_values[n_columns];
    unsigned char z_values[n_columns];
    for(int n = 0; n < n_columns; n++) {
      addresses[n] = current_address;
      x_values[n] = cards[n][i].x;
      y_values[n] = cards[n][i].y;
      z_values[n] = cards[n][i].z;
    }
    sendAddress(addresses);
    sendX(x_values);
    sendY(y_values);
    sendZ(z_values);
  }
}

void sendStartByte() { // Anything between [251, 255]*-*
  unsigned char initializers[n_columns];
  for(int i = 0; i < n_columns; i++) {
    initializers[i] = 251;
  }
  sendByte(initializers);
  #ifdef DEBUG
    Serial.println("sendStartByte:");
  #endif
}

void sendAddress(unsigned char addresses[n_columns]) { // Numbers between [1, 12]
  sendByte(addresses);
  #ifdef DEBUG
    Serial.println("sendAddress:");
  #endif
}

void sendX(unsigned char values[n_columns]) { // Numbers between [100, 200]
  sendByte(values);
  #ifdef DEBUG
    Serial.println("sendX:");
  #endif
}

void sendY(unsigned char values[n_columns]) { // Numbers between [100, 200]
  sendByte(values);
  #ifdef DEBUG
    Serial.println("sendY:");
  #endif
}

void sendZ(unsigned char values[n_columns]) { // Numbers between [100, 200]
  sendByte(values);
  #ifdef DEBUG
    Serial.println("sendZ:");
  #endif
}

void sendByte(unsigned char values[n_columns]) {
  // Start bit
  setAllLow();
  delayMicroseconds(bit_duration);

  // 8-bit
  for (int bt = 0; bt < 8; bt++)
  {
    for (int pin = 22; pin < (22 + n_columns); pin++) {
      unsigned char bit_val = ((values[pin - 22] >> bt) & 0x01);
      if(bit_val == 1) {
        digitalWrite(pin, HIGH);
        #ifdef DEBUG
          Serial.println("1");
        #endif
      } else {
        digitalWrite(pin, LOW);
        #ifdef DEBUG
          Serial.println("0");
        #endif
      }
    }
    delayMicroseconds(bit_duration);
  }

  // Stop bit
  setAllHigh();
  delayMicroseconds(bit_duration);
  #ifdef DEBUG
    Serial.println("_______");
  #endif
}

void setAllLow() {
  for (int pin = 22; pin < (22 + n_columns); pin++) {
    digitalWrite(pin, LOW);
  }
}

void setAllHigh() {
  for (int pin = 22; pin < (22 + n_columns); pin++) {
    digitalWrite(pin, HIGH);
  }
}

unsigned int rand_interval(unsigned int min, unsigned int max)
{
    int r;
    const unsigned int range = 1 + max - min;
    const unsigned int buckets = RAND_MAX / range;
    const unsigned int limit = buckets * range;

    /* Create equal size buckets all in a row, then fire randomly towards
     * the buckets until you land in one of them. All buckets are equally
     * likely. If you land off the end of the line of buckets, try again. */
    do
    {
        r = rand();
    } while (r >= limit);

    return min + (r / buckets);
}
