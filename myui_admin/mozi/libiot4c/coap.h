
// Ĭ�Ϸ���˿�UDP
#define  Port  5683
// Ĭ��DTLS����˿�
#define SecurePort  5684
// Ĭ�Ϸ���˿�TCP
#define TcpPort  5685
// ͷ�������ݷָ�����
#define HeaderEnd  0xFF

/**
 * CoAP�� 
 */
typedef struct CoAPPackage {

} coappackage;
/*
* 
*/
typedef struct CoAPOption {

} coapoption;

typedef struct CoAPOptionValue {

} coapoptionvalue;

CoAPPackage parse(byte[] * data) {

}

byte[] pack(CoAPPackage *pack){

}