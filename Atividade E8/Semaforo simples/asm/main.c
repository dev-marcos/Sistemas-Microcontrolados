//E8d : Semáforo simples para carros com botão para pedestre:
// • O semáforo permanece em verde para o carros até que,
// • Seja pressionado o botão de pedestre,
// • aguarda 3 segundos;
// • Sinal passa para amarelo por 2 s;
// • Vermelho por 3 s;
// • Verde até que o botão seja pressionado novamente.

// PA0 -> LED Vermelho
// PA2 -> LED Amarelo
// PA4 -> LED Verde
// PA7 -> push-button para o terra


// **** Versão ASM ****
extern void MainAsm(void);

int main(void){
	MainAsm(); // Entra na label principal de parte_assembly.s e não retorna pois lá tem um loop infinito

}
