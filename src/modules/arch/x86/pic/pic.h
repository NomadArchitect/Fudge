void pic_routine00(void);
void pic_routine01(void);
void pic_routine02(void);
void pic_routine03(void);
void pic_routine04(void);
void pic_routine05(void);
void pic_routine06(void);
void pic_routine07(void);
void pic_routine08(void);
void pic_routine09(void);
void pic_routine0A(void);
void pic_routine0B(void);
void pic_routine0C(void);
void pic_routine0D(void);
void pic_routine0E(void);
void pic_routine0F(void);
unsigned int pic_setroutine(unsigned int irq, void (*routine)(unsigned int irq));
unsigned int pic_unsetroutine(unsigned int irq);
void pic_disable(void);
