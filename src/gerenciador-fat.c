#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void limpar_disco(void);
void limpar_tabelas(void);
void criar_arquivo(void);
void apagar_arquivo(void);
void desalocar_disco(int x);
void ler(void);
void ler_disco(int x);
void imprimir_tudo(void);
void gravar_arquivo(void);
void gravar_arquivo_no_disco(int x, char *y, int z);
int proxima_livre(void);

typedef struct tab //Estrutura que representa a tabela de diretórios.
{
	char nome[16];
	int bloco_inicial;
}Tab;

char *disco;			
int *tabela_fat;	    
Tab *tabela_diretorio;  

int tamanho_disco = 0;
int tamanho_bloco = 0;
int quantidade_bloco = 0;

int main()
{
	printf("Digite o tamanho do bloco: "); //Pegando a quantidade de bits de um bloco.
	scanf("%i", &tamanho_bloco);
	setbuf(stdin,NULL);
	printf("Digite a quantidade de blocos: "); //Pegando quantidade de blocos.
	scanf("%i", &quantidade_bloco);
	setbuf(stdin,NULL);

	tamanho_disco = tamanho_bloco * quantidade_bloco; //Guardando quantidade de bits do disco.

	disco = (char *) malloc(tamanho_disco * sizeof(char));	//Alocando memória para guardar as strings do disco.
	tabela_fat = (int *) malloc(quantidade_bloco * sizeof(int)); //Alocando memória para guardar tabela fat.
	tabela_diretorio = (Tab *) malloc(quantidade_bloco * sizeof(Tab)); //Alocando memória para estrutura tabela de diretórios.

	limpar_disco(); // Escreve '.' no disco para melhor visualização.
	limpar_tabelas(); // Define números nas tabelas para melhor visualização. 
	
	int opcao = 0;

	while(opcao != 6) // Loop que roda o menu.
	{
		system("clear");
		printf(" _______________________________ \n");
		printf("|************* MENU ************|\n");
		printf("|                               |\n");
		printf("|        [1] Criar              |\n");
		printf("|        [2] Apagar             |\n");
		printf("|        [3] Ler                |\n");
		printf("|        [4] Gravar             |\n");
		printf("|        [5] Imprimir tudo      |\n");
		printf("|        [6] Sair               |\n");
		printf("|_______________________________|\n");
		printf("\n");

		printf("Digite a opcao desejada: ");
		scanf("%i", &opcao);
		setbuf(stdin,NULL);

		if(opcao<1||opcao>6)
		{
			printf("\nOpção Inválida!\n\n");
			getchar();			
		}
		else
		{
			switch(opcao)
			{
				case 1:
					criar_arquivo();
					break;

				case 2:
					apagar_arquivo();
					break;

				case 3:
					ler();
					break;

				case 4:
					gravar_arquivo();
					break;

				case 5:
					imprimir_tudo();
					break;
			}
		}	
	}	
	free(disco);//Libera o espaço de memória que estava guardando o disco.
	free(tabela_fat);//Libera o espaço de memória que estava guardando a tabela fat.
	free(tabela_diretorio);//Libera o espaço de memória que estava guardando a tabela de diretórios.
	return 0;
}

void limpar_disco(void)// Escreve '.' no disco para melhor visualização.
{
	for(int i = 0; i < tamanho_disco; i++)
	{
		disco[i] = '.';
	}
}

void limpar_tabelas(void)// Define números nas tabelas para melhor visualização.
{
	for(int i = 0; i < quantidade_bloco; i++)
	{
		tabela_fat[i] = -2;
		tabela_diretorio[i].bloco_inicial = -1;
		strcpy(tabela_diretorio[i].nome, "...");
	} 
}

void criar_arquivo(void)// Cria a casca do arquivo e coloca -2 como bloco inicial para identificação da casca.
{	
	char nome[16];

	printf("Digite o nome do arquivo (sem espacos!): ");//Pede nome do arquivo para identificação.
	scanf("%s", nome);
	setbuf(stdin,NULL);

	for(int i = 0; i < quantidade_bloco; i++)
	{	
		if(tabela_diretorio[i].bloco_inicial == -1)//Procura uma posição sem arquivo criado ou gravado na tabela de diretórios.
		{
			strcpy(tabela_diretorio[i].nome, nome);//Coloca nome do arquivo criado na tabela de diretórios.
			tabela_diretorio[i].bloco_inicial = -2;//Coloca como bloco inicial -2 para o arquivo criado.
			printf("\nArquivo criado!"); 
			break;// Quebra o laço, pois não há mais necessidade de procurar posição.
		}
	}
	getchar();
}

void apagar_arquivo(void)
{
	char nome[16];
	int tam = strlen(nome);
	int verificador = -1;

	printf("Digite o nome do arquivo que sera apagado: ");//Pede o nome do arquivo que será apagado.
	scanf("%s", nome);
	setbuf(stdin,NULL);

	for(int i = 0; i < quantidade_bloco; i++)
	{
		verificador = strncmp(tabela_diretorio[i].nome, nome, tam);//Compara o nome do arquivo com o digitado para encontrar sua posição na tabela.	
		if(verificador == 0)
		{
			desalocar_disco(tabela_diretorio[i].bloco_inicial);//Chama função para apagar disco passando bloco inicial do arquivo.
			tabela_diretorio[i].bloco_inicial = -1;//Coloca -1 na tabela de diretórios para dizer que o arquivo foi apagado.
			strcpy(tabela_diretorio[i].nome, "...");//Sobrescreve o nome do arquivo com "..." para melhor visualização.
			printf("\nArquivo apagado!\n");
			getchar();
			break;//Quebrar o laço, pois o arquivo já foi encontrado e apagado.
		}		
	}
}

void desalocar_disco(int bloco_inicial)//Função recursiva que coloca '.' nos blocos reservados para o arquivo que foi apagado.
{	
	int bloco_secundario = tabela_fat[bloco_inicial];//Guarda a próxima posição.
	int posicao_inicial = bloco_inicial * tamanho_bloco;//Guarda a posição inicial do bloco no disco.

	if(bloco_inicial != -1)//Condição de parada para identificar o bloco final. 
	{
		for(int i = posicao_inicial; i < posicao_inicial + tamanho_bloco; i++)
		{
			disco[i] = '.';
		}
		tabela_fat[bloco_inicial] = -2;//Limpa a posição da tabela fat, para melhor visualização.

		desalocar_disco(bloco_secundario);//Atualiza o bloco inicial na chamada recursiva.
	}
}

void ler(void)
{
	char nome[16];
	int bloco_inicial = -1;

	printf("Digite o nome do arquivo: ");//Pede nome do arquivo que será lido.
	scanf("%s", nome);
	setbuf(stdin,NULL);

	int tam = strlen(nome);

	for(int i = 0; i < quantidade_bloco; i++)
	{
		if(strncmp(tabela_diretorio[i].nome, nome, tam) == 0)//Compara o nome digitado com o da tabela de diretórios.	
		{	
			bloco_inicial = tabela_diretorio[i].bloco_inicial;//Guarda bloco inicial do arquivo que será lido.
			break;//Sai do laço, pois o arquivo já foi encontrado.
		}
	}

	switch(bloco_inicial)//Com base no bloco inicial, identifica se o arquivo foi gravado.
	{
		case -1:
			printf("\nArquivo nao encontrado!");
			getchar();
		break;

		case -2:
			printf("\nArquivo nao gravado!");
			getchar();
		break;

		default:		
			printf("\nArquivo gravado: ");
			ler_disco(bloco_inicial);//Chama função que escreve conteúdo do arquivo na tela, passando bloco inicial.
			getchar();
	}	
}

void ler_disco(int bloco_inicial)//Função recursiva que escreve na tela o conteúdo do arquivo gravado no disco, com base no bloco inicial.
{	
	int bloco_secundario = tabela_fat[bloco_inicial];//Guarda a próxima posição.
	int posicao_inicial = bloco_inicial * tamanho_bloco;//Guarda a posição inicial do bloco no disco.

	if(bloco_inicial != -1)//Condição de parada para identificar o bloco final.
	{
		for(int i = posicao_inicial; i < posicao_inicial + tamanho_bloco; i++)
		{
			printf("%c", disco[i]);
		}
		ler_disco(bloco_secundario);//Atualiza o bloco inicial na chamada recursiva.
	}
}

void imprimir_tudo(void)//Imprime as informações do disco e das tabelas na tela.
{
	printf("\n\n");

	printf("Disco: ");
	for(int i = 0; i < tamanho_disco; i++)
	{
		printf("%c ", disco[i]);
	}

	printf("\n\n");

	printf("Fat: ");
	for(int j = 0; j < quantidade_bloco; j++)
	{
		printf("[%i] ", tabela_fat[j]);
	}

	printf("\n\n");

	printf("Nome: ");
	for(int k = 0; k < quantidade_bloco; k++)
	{
		printf("[%s] ", tabela_diretorio[k].nome);
	}

	printf("\n\n");

	printf("Bloco inicial: ");
	for(int l = 0; l < quantidade_bloco; l++)
	{
		printf("[%i] ", tabela_diretorio[l].bloco_inicial);
	}

	printf("\n\n");
	getchar();
}

int proxima_livre(void)//Procura e retorna uma posição livre da tabela fat.
{
	int posicao = 0;

	while(tabela_fat[posicao] != -2)
	{
		posicao++;
	}
	return posicao;
}

void gravar_arquivo(void)//Grava o conteudo do arquivo no dico e liga na sua repectiva casca.
{	
	int i = 0, j = 0;
	char nome[16];
	char conteudo_arquivo[64];

	printf("Digite o nome do arquivo: ");//Pede o nome do arquivo que será gravado.
	scanf("%s", nome);
	setbuf(stdin,NULL);

	printf("Digite o conteudo do arquivo (sem espacos!): ");//Pede o conteudo do arquivo que será gravado.
	scanf("%s", conteudo_arquivo);
	setbuf(stdin,NULL);

	int bloco_inicial = -1;
	int tam = strlen(conteudo_arquivo);
	int tam2 = strlen(nome);
	int blocos_ocupados = 0;//Guarda a quantidade de blocos necessária para se guardar o conteúdo do arquivo. 
	int verificador = 0;

	for(i = 0; i < quantidade_bloco; i++)
	{
		if((strncmp(tabela_diretorio[i].nome, nome, tam2) == 0 && tabela_diretorio[i].bloco_inicial == -2))//Compara o nome digitado com o nome do arquivo e verificado se a casca está vazia. 	
		{	                                                                                               
			switch(tam % tamanho_bloco)//Calcula a quantidade de blocos necessária para guardar o conteúdo do arquivo.
			{			
				case 0:
					blocos_ocupados = (tam / tamanho_bloco);
				break;

				default:
					blocos_ocupados = (tam / tamanho_bloco) + 1;
				break;
			}

			for(j = 0; j < quantidade_bloco; j++)
			{
				if(tabela_fat[j] == -2)//Guarda o valor do bloco inicial e coloca o valor da próxima posição na tabela fat.
				{	
					bloco_inicial = j;//Guarda bloco inicial.
					tabela_fat[j] = -1;//Coloca -1 na tabela fat para identificar bloco final.
					blocos_ocupados--;//Diminui a quantidade de blocos. 
					if(blocos_ocupados > 0)//Troca o -1 da tabela fat pelo proximo bloco livre se o conteúdo do arquivo necessitar.
					{
						tabela_fat[j] = proxima_livre();		
					}
					break;
				}
			}
			tabela_diretorio[i].bloco_inicial = bloco_inicial;//Guarda o bloco inicial do arquivo na tabela de diretórios.
			verificador = 1;//Libera gravação do arquivo no disco.
			break;
		}
	}

	switch(verificador)//Verifica se o arquivo foi liberado para ser gravado no disco.
	{
		case 0:
			printf("\nArquivo nao encontrado!");
			getchar();
		break;

		case 1:
			i=0;
			while(blocos_ocupados > 0)//Condição de parada para preencher tabela fat.
			{
				if(tabela_fat[i] == -2)//Preenche a tabela fat com as posições do próximo bloco.
				{
					tabela_fat[i] = -1;//Coloca -1 na tabela fat para identificar bloco final.
					blocos_ocupados--;//Diminui a quantidade de blocos.
					if(blocos_ocupados > 0)//Troca o -1 da tabela fat pelo proximo bloco livre se o conteúdo do arquivo necessitar.
					{
						tabela_fat[i] = proxima_livre();		
					}
				}		
				i++;
			}

			char *ponteiro;
			ponteiro = conteudo_arquivo;//Faz ponteiro apontar primeira posição do vetor 'arquivo alocado' que foi digitado no começo da função.

			gravar_arquivo_no_disco(bloco_inicial, ponteiro, tam);//Grava arquivo no disco passando ponteiro como parâmetro.

			printf("\nArquivo alocado!\n");
			getchar();
		break;
	}
}

void gravar_arquivo_no_disco(int bloco_inicial, char *ponteiro, int tamanho)//Função recursiva que grava string digitada nos blocos reservados para o conteúdo do arquivo.
{	
	if(tamanho > 0)//Condição de parada usando tamanho da string.
		for(int i = 0; i < tamanho_bloco; i++)
		{	
			disco[bloco_inicial * tamanho_bloco + i] = *ponteiro;//Passa caractere por caractere de uma string para outra na recursão.
			tamanho--;//Decresce o tamanho, pois ja foi copiado um caractere.
			if(tamanho == 0)//Quebra o laço se o tamanho da palavra for atingido.
				break;
			ponteiro++;//Faz o ponteiro apontar para a posição de memória ao seu lado (próxima).
		}
	if(tamanho > 0)//Se ainda houver caractere a ser copiado, atualiza a informações chamando recursivamente. 
		gravar_arquivo_no_disco(tabela_fat[bloco_inicial], ponteiro, tamanho);
}
