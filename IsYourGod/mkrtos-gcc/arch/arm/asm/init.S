.syntax unified
.cpu cortex-m3
.thumb

.global Init
.section .text.Init
.type Init, %function
Init:
CPSID   I
	//��������pendsvΪ������ȼ�
	//����pendsv���ж����ȼ�
	ldr r0,=0xE000ED22
	//������ȼ�,pendsv��systick���ȼ�
	ldr r1,=0xa0a0
	strh r1,[r0]

	ldr r0,=0xE000ED1F
	ldr r1,=0xf0
	strb r1,[r0]
	//����pspΪ0,�����ж��Ƿ��һ���������
	MOVS R0, #0 //R0 = 0
	MSR PSP, R0 //PSP = R0

	LDR R0, =0xE000ED04
	LDR R1, =0x10000000
	//����pendsv�ж�
	STR R1, [R0]
	//���ж�
    CPSIE I ;
	//�û��߳�ģʽ,�û��Է���Ȩģʽ����
	MOV R0,#1
	MSR CONTROL, R0
	BX LR


/**
* @brief ���жϺ���
*/

.global SVC_Handler
.section .text.SVC_Handler
.type SVC_Handler, %function
SVC_Handler:
	TST.W LR, #4
	ITE EQ
 	MRSEQ R0, MSP
	MRSNE R0, PSP
	//��ջr4-r11
	SUB R0, R0, #0x20
	STM R0, {R4-R11}
    MOV R1, R7
	B svcHandler