#ifndef   __PORTDEF_H__
#define   __PORTDEF_H__

#define DIPSW2_PIN            12
#define DIPSW1_PIN            13

#define STLED                 21

#define U1_TX                 17
#define U1_RX                 18

#define SOFT_TX               15
#define SOFT_RX               16
// #define MESH_LED              41
// #define RS485_LED             42


// #define MESH_LED_ON           digitalWrite(MESH_LED,LOW)
// #define MESH_LED_OFF          digitalWrite(MESH_LED,HIGH)
// #define MESH_LED_TOGGLE       digitalWrite(MESH_LED,!digitalRead(MESH_LED))

// #define RS485_LED_ON          digitalWrite(RS485_LED,LOW)
// #define RS485_LED_OFF         digitalWrite(RS485_LED,HIGH)
// #define RS485_LED_TOGGLE      digitalWrite(RS485_LED,!digitalRead(RS485_LED))


#define DIPSW2_IN             digitalRead(DIPSW2_PIN)
#define DIPSW1_IN             digitalRead(DIPSW1_PIN)


#endif