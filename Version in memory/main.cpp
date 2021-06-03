/*
Trabalho de Estrutura de Dados - GCC216
Ordenacao em Memoria Secundaria, com Sequence Set e Arvore B+
Copyright 2020 by Chrystian Arriel Amaral, Debora Rossini Martins Cardoso e Thiago Salles Santos
Arquivo main.cpp: programa principal que instancia os objetos SequenceSet e leCSV, definindo opcoes de leitura
do arquivo .csv e de tratamento dos dados no arquivo binario do sequence set
*/

#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <cstdlib>
#include <stdexcept>
#include <time.h>

using namespace std;

struct pacote{
    unsigned indice;
    float tempo;
    char origem[40];
    char destino[40];
    char protocolo[18];
    unsigned tamanho;
    char infomarcao[1650];
};

#include "sequence_set.h"
#include "leCSV.h"

void menuInicial(bool& caso);
void menuTratamento(int& control);
void menuFinaliza();
void leituraDoCSV(SequenceSet sequence, int aux1, int aux2, bool& caso);

int main(){
    unsigned aux1 = 519091, aux2 = 605605;
    int control;
    bool caso=false;
    SequenceSet sequence(caso);

    menuInicial(caso);  // Inicio do programa: ler o .csv e add no binario, pelo sequence set
    if(!caso){
        leituraDoCSV(sequence, aux1, aux2, caso);
    }
    try{
        //Menu de tratamento
        control = -1;
        do{
            menuTratamento(control);

            if(control == 1){
                sequence.insere(caso);
            }else if(control == 2){
                sequence.busca();
            }else if(control == 3){
                sequence.atualiza();
            }else if(control == 4){
                sequence.criaTxt();
            }else if(control < 0 or control > 4){
                cout << "* Insira um comando valido!" << endl;
            }
        }while(control != 0);
    }
    catch (std::runtime_error& e) {
        std::cout << e.what() << std::endl;
    }

    menuFinaliza();
    return 0;
}

void menuInicial(bool& caso){
    cout << "################ Sequence Set com Arvore B+ - Um Novo Mundo ################" << endl << endl;
    cout << "                       -> Versao Arvore B+ em memoria <-                    " << endl << endl;
    if(caso){
        cout << "* Existe um arquivo pronto, deseja refazer a leitura?" << endl << endl;
        cout << "* [0] Sim" << endl;
        cout << "* [1] Nao" << endl << endl;
        cout << "-> Digite a opcao: ";
        cin >> caso;
    }else{
        caso = false;
    }
}

void leituraDoCSV(SequenceSet sequence, int aux1, int aux2, bool& caso){
    sequence.setInicial();

    int control;
    cout << endl << "|| Iniciando leitura do arquivo \"captura_pacotes.csv\" ||" << endl << endl;
    cout << "* [0] Sem funcao empresta" << endl;
    cout << "* [1] Com funcao empresta (mais eficiente em uso de memoria secundaria)" << endl << endl;
    cout << "-> Digite a opcao: ";
    cin >> caso;
    cout << endl << "* [1] - Definir trecho" << endl;
    cout << "* [2] - Trecho padrao (519091, 605605)" << endl << endl;
    cout << "-> Digite a opcao: ";
    cin >> control;
    cout << endl;

    // Leitura do "captura_pacotes.csv"
    if(control == 2){  // trecho padrao
        leCSV leitura(sequence, aux1, (aux2-aux1), caso);
    }else if(control == 1){  // trecho definido
        cout << "-> Digite o inicio do trecho: ";
        cin >> aux1;
        cout << "-> Digite o fim do trecho: ";
        cin >> aux2;
        cout << endl;
        if(aux1 < aux2){
            leCSV leitura(sequence, aux1, (aux2-aux1), caso);
        }else{
            throw runtime_error("********Trecho invalido*******");
        }
    }else{
        throw runtime_error("*******Comando invalido*******");
    }
    sequence.criaTxt();
}

void menuTratamento(int& control){
    cout << "--------------- Menu de Opcoes ---------------" << endl << endl;
    cout << "* [1] - Inserir" << endl;
    cout << "* [2] - Buscar" << endl;
    cout << "* [3] - Atualizar" << endl;
    cout << "* [4] - Cria novo Txt do Sequence Set" << endl;
    cout << "* [0] - Finalizar programa" << endl << endl;
    cout << "-> Digite a opcao: ";
    cin >> control;
    cout << endl << endl;
}

void menuFinaliza(){
    cout << "######################## Finalizando #########################" << endl << endl;
    cout << "-> Program by Chrystian Arriel, Debora Rossini and Thiago Salles" << endl << endl;
    cout << "-> GCC216 - Estruturas de Dados" << endl;
    cout << "-> DCC - UFLA" << endl;
    cout << "-> 2020/01" << endl << endl;
    cout << "##############################################################" << endl;
}
