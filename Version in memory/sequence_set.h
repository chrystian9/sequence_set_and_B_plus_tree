/*
Trabalho de Estrutura de Dados - GCC216
Ordenacao em Memoria Secundaria, com Sequence Set e Arvore B+
Copyright 2020 by Chrystian Arriel Amaral, Debora Rossini Martins Cardoso e Thiago Salles Santos
Arquivo sequence_set.h: definição e funções da class SequenceSet que ordena e trata os dados em um arquivo binario.
Versão de arvore em memoria.
*/

#include "arvoreBplus.h"
class SequenceSet{
    private:
        arvoreBplus arvore;
        // Maximo de elementos por packet
        unsigned MAXelements;
        // Estrutura do packet
        struct Packet{
            unsigned id;  // indice do packet (referencia para sua posicao no arquivo bin)
            unsigned quantElements;  // quantidade de elementos no vetor do packet
            int nextPacket;  // proximo packet (com base na ordenacao dos elementos): -1 indica que nao ha proximo
            int laterPacket;  // packet anterior (com base na ordenacao dos elementos): -1 indica que nao ha anterior
            pacote vElementos[80];  // vetor de elementos
        };
        // Estrutura do cabecalho
        struct Cabecalho{
            unsigned quantPackets;  // quantidade de packets no arquivo
            unsigned nextPacket;  // proximo packet a ser criado (ou um vazio)
            unsigned beginPacket;  // primeiro packet da ordenacao
            unsigned endPacket;  // ultimo packet da ordenacao
            unsigned quantElements;  // quantidade total de elementos no arquivo bin
        };
        // Variavel padrao para o cabecalho
        Cabecalho cabecalho;
        // Set o cabecalho com configuracoes iniciais
        void criaCabecalho(Cabecalho &cabecalho);
        // Set o packet com configuracoes iniciais
        void criaPacket(Packet &packet, int later, unsigned id, int next);
        // Insere elemento no vetor de um packet especifico
        void insereNoVetor(pacote& newPacote, Packet& packet1);
        // Divide packet em dois, o antigo e um novo (ou um vazio)
        void dividePacket(Packet& packet1, Packet& packet2);
        // Passa elemento para o packet posterior ou anterior
        void empresta(Packet& packet1, Packet& packet2, bool qualPacket);
        // Insere elemento no packet especifico
        void insereNoPacket(Packet& packet1, pacote& newPacote, bool casoEmpresta);
        // Procura o primeiro packet que o elemento é menor ou igual ao ultimo do vetor
        //unsigned buscaPacket(pacote& umPacote, Packet& packet1, bool casoInsercao);
        // Retorna se o primeiro parametro é menor (true) ou maior (false) que o segundo parametro (igual retorna falso)
        bool compara(pacote& pacote1, pacote& pacote2, bool casoInsercao);
        // Le um packet do arquivo bin com base no indice do packet (pos)
        void lePacketArq(Packet& packet, unsigned pos);
        // Escreve um packet no arquivo bin com base no indice do packet (pos)
        void escrevePacketArq(Packet& packet, unsigned pos);
        // Le o cabecalho do arquivo bin
        void leCabecalhoArq();
        // Escreve o cabecalho no arquivo bin
        void escreveCabecalhoArq();
        // Retorna se os elementos são iguais com base em um campo especifico
        bool comparaIgualdade(pacote elemento1, pacote elemento2, int campo);
        int buscaEficiente(pacote pacoteBusca, Packet& packet1);
        void buscaElementos(pacote elementoBusca, int campo);
        // Pega elemento por indice
        int pegaElementoPorIndice(Packet& packet1, int indiceBusca);
        void criaArvore();
    public:
        // Construtor: inicia o arquivo com o cabecalho e um primeiro packet
        SequenceSet(bool& existe);
        // Insere um elemnto no arquivo
        void insereElemento(pacote newPacote, bool casoEmpresta);
        // Gera um arquivo txt com os dados do arquivo bin
        void criaTxt();
        // Atualiza um elemento (set qual indice)
        void atualiza();
        // Busca um elemento (set qual elemento)
        void busca();
        // Insere elemento no arquivo (seta elemento)
        void insere(bool casoInsercao);
        void setInicial();
};

SequenceSet::SequenceSet(bool& existe){
    MAXelements = 80;
    
    fstream arq("arq_comDados.bin", ios::in | ios::out | ios::binary);
    if (arq) {
        criaArvore();
        existe = true;
    }
    arq.close();
};

void SequenceSet::criaArvore(){
    Packet packet1;
    this->leCabecalhoArq();
    lePacketArq(packet1, cabecalho.beginPacket);
    
    do{
        lePacketArq(packet1, packet1.nextPacket);
        arvore.promove(packet1.vElementos[0], packet1.laterPacket, packet1.id); 
    }while(packet1.nextPacket != -1);
}

void SequenceSet::setInicial(){
    fstream arqNovo("arq_comDados.bin", ios::in | ios::out | ios::binary | ios::trunc);
    if(not(arqNovo)){
        throw runtime_error("Erro ao criar o arquivo bin (Arquivo com dados)");
    }
    arqNovo.close();

    Packet packet1;
    criaCabecalho(cabecalho);
    criaPacket(packet1, -1, 0, -1);  // Seta o packet inicial, sem anterior e posterior
    escrevePacketArq(packet1, 0);
    cabecalho.quantPackets++;
    escreveCabecalhoArq();
}

void SequenceSet::atualiza(){
    Packet packet1;
    pacote buscado;
    std::string dado;
    std::cout<<"Entre com o destino a ser procurado: ";
    std::cin.ignore();
    std::getline(std::cin, dado);
    std::strcpy(buscado.destino, dado.c_str());
    std::cout<<"Entre com o indice: ";
    std::cin>>buscado.indice;
    int pos = this->buscaEficiente(buscado, packet1);
    if(pos == -1){
        std::cout<<"Nao encontrado"<<std::endl;
    }else{
        int escolha;
        bool atualizou = false;
        do{
            std::cout<<"Elemento: "<<std::endl;
            std::cout<<packet1.vElementos[pos].indice<<" | "<<packet1.vElementos[pos].tempo<<" | "<<
                packet1.vElementos[pos].origem<<" | "<<packet1.vElementos[pos].destino<<" | "<<
                packet1.vElementos[pos].protocolo<<" | "<<packet1.vElementos[pos].tamanho<<" | "<<
                packet1.vElementos[pos].infomarcao<<std::endl;
            std::cout<<"Escolha a atualizacao a ser feita: "<<std::endl;
            std::cout<<"[1] - Tempo"<<std::endl;
            std::cout<<"[2] - Origem"<<std::endl;
            std::cout<<"[3] - Protocolo"<<std::endl;
            std::cout<<"[4] - Tamanho"<<std::endl;
            std::cout<<"[5] - Informacao"<<std::endl;
            std::cout<<"[6] - Sair"<<std::endl;
            std::cin>>escolha;
            if(escolha >=1 and escolha <= 5){
                atualizou = true;
            }
            if(escolha == 1){
                std::cout<<"Digite o novo tempo: "<<std::endl;
                std::cin>>packet1.vElementos[pos].tempo;
            }else if(escolha == 2){
                std::cout<<"Digite a nova origem: "<<std::endl;
                std::cin.ignore();
                std::getline(std::cin, dado);
                std::strcpy(packet1.vElementos[pos].origem, dado.c_str());
            }else if(escolha == 3){
                std::cout<<"Digite o novo protocolo: "<<std::endl;
                std::cin.ignore();
                std::getline(std::cin, dado);
                std::strcpy(packet1.vElementos[pos].protocolo, dado.c_str());
            }else if(escolha == 4){
                std::cout<<"Digite o novo tamanho: "<<std::endl;
                std::cin>>packet1.vElementos[pos].tamanho;
            }else if(escolha == 5){
                std::cout<<"Digite a nova informacao: "<<std::endl;
                std::cin.ignore();
                std::getline(std::cin, dado);
                std::strcpy(packet1.vElementos[pos].infomarcao, dado.c_str());
            }
        }while(escolha !=6);
        if(atualizou){
            do{
                std::cout<<"Deseja salvar a atualizacao? "<<std::endl;
                std::cout<<"[1] - Sim"<<std::endl;
                std::cout<<"[2] - Nao"<<std::endl;
                std::cin>>escolha;
                if(escolha == 1){
                    this->escrevePacketArq(packet1, packet1.id);
                    this->criaTxt();
                    escolha = 2;
                }
            }while(escolha != 2);
        }
    }
}

// Set por qual campo e qual o valor dele para ser buscado
void SequenceSet::busca(){
    pacote buscado;
    std::string dado;
    int escolha;
    do{
        std::cout<<"Busca Dado(s)"<<std::endl;
        std::cout<<"[1] - Busca eficiente"<<std::endl;
        std::cout<<"[2] - Busca ineficiente"<<std::endl;
        std::cout<<"[3] - Sair"<<std::endl<<std::endl;
        std::cin>>escolha;
        if(escolha == 1){
            Packet packet1;
            std::cout<<"->Entre com o destino a ser procurado: ";
            std::cin.ignore();
            std::getline(std::cin, dado);
            std::strcpy(buscado.destino, dado.c_str());
            std::cout<<"->Entre com o indice: ";
            std::cin>>buscado.indice;
            std::cout<<endl;
            clock_t inicio = clock();
            int pos = this->buscaEficiente(buscado, packet1);
            clock_t fim = clock();
            if(pos == -1){
                std::cout<<"* Nao encontrado (tempo: " << (fim - inicio)/(double) CLOCKS_PER_SEC << " segundos)" << endl << endl;
            }else{
                std::cout<<"* Elemento encontrado (tempo: " << (fim - inicio)/(double) CLOCKS_PER_SEC << " segundos)" << endl << endl;
                std::cout<<packet1.vElementos[pos].indice<<" | "<<packet1.vElementos[pos].tempo<<" | "<<
                packet1.vElementos[pos].origem<<" | "<<packet1.vElementos[pos].destino<<" | "<<
                packet1.vElementos[pos].protocolo<<" | "<<packet1.vElementos[pos].tamanho<<" | "<<
                packet1.vElementos[pos].infomarcao<<std::endl;
            }
        }else if(escolha == 2){
            int aux;
            cout << "-> Campos" << endl << endl;
            cout << "[1] - Indice " << endl;
            cout << "[2] - Tempo " << endl;
            cout << "[3] - Origem " << endl;
            cout << "[4] - Destino " << endl;
            cout << "[5] - Protocolo " << endl;
            cout << "[6] - Tamanho " << endl;
            cout << "[7] - Informacao " << endl;
            cout << "[8] - Busca especifica e eficiente (pelo destino e o indice) " << endl << endl;
            cout << "-> Digite a opcao: ";
            cin >> aux;
            cin.ignore();
            if(aux >= 1 and aux <= 7){
                string auxString;
                if(aux == 1){
                    cout << "-> Digite o indice: ";
                    cin >> buscado.indice;
                }if(aux == 2){
                    cout << "-> Digite o tempo: ";
                    cin >> buscado.tempo;
                }if(aux == 3){
                    cout << "-> Digite a origem: ";
                    getline(cin, auxString);
                    strcpy(buscado.origem, auxString.c_str());
                }if(aux == 4){
                    cout << "-> Digite o destino: ";
                    getline(cin, auxString);
                    buscado.indice = 0;
                    strcpy(buscado.destino, auxString.c_str());
                }if(aux == 5){
                    cout << "-> Digite o protocolo: ";
                    getline(cin, auxString);
                    strcpy(buscado.protocolo, auxString.c_str());
                }if(aux == 6){
                    cout << "-> Digite o tamanho: ";
                    cin >> buscado.tamanho;
                }if(aux == 7){
                    cout << "-> Digite a informacao: ";
                    getline(cin, auxString);
                    strcpy(buscado.infomarcao, auxString.c_str());
                }
                this->buscaElementos(buscado, aux);
            }else{
                throw runtime_error("*******Comando invalido*******");
            }
        }else{
            std::cout<<"Entre com um elemento valido!"<<std::endl;
        }
    }while(escolha != 3);
}

int SequenceSet::buscaEficiente(pacote pacoteBusca, Packet& packet1){
    cout << "Buscando..." << endl;
    int posicao = this->arvore.busca(pacoteBusca, -1, -1);//faço a busca do elemento pela arvore
    this->lePacketArq(packet1, posicao);
    unsigned c = 0;
    
    while(c < packet1.quantElements){//procuro onde o elemento esta no packet
        if(not(this->comparaIgualdade(packet1.vElementos[c], pacoteBusca, 1))){
            c++;
        }else{
            return c;
        }
    }
    return -1;
}

// Busca elemento com base em um campo especifico e escreve em um arquivo txt o resultado
void SequenceSet::buscaElementos(pacote elementoBusca, int campo){//busca ineficiente
    Packet packet1;
    bool result = false; // marca resultado positivo
    this->leCabecalhoArq();

    packet1.nextPacket = this->cabecalho.beginPacket; // pega a posicao do primeiro packet
    ofstream arqBusca("dadosBusca_caso"+std::to_string(campo)+".txt", ios::out);  // cria o arquivo txt
    if (not(arqBusca)) {
        throw runtime_error("Erro ao criar o arquivo txt(buscaElemento)");
    }
    clock_t inicio = clock();
    cout << "Buscando..." << endl;
    if(campo == 4){  // caso de busca por chave primaria (destino)
        int pos = this->arvore.busca(elementoBusca, -1, -1);
        lePacketArq(packet1, pos);
        unsigned cont=0;
        bool vigia = true;
        while(vigia){
            // Comeca no inicio do vetor procurando elementos iguais ao buscado
            if(this->comparaIgualdade(packet1.vElementos[cont], elementoBusca, 4)){// Se o elemento foi igual, escreve no arquivo de resultado
                arqBusca<<"Packet: "<<packet1.id<<" Posicao: "<<cont<<endl;
                arqBusca<<"Dados: "<<packet1.vElementos[cont].indice<<" || ";
                arqBusca<<packet1.vElementos[cont].tempo<<" || "<<packet1.vElementos[cont].origem;
                arqBusca<<" || "<<packet1.vElementos[cont].destino<<" || "<<packet1.vElementos[cont].protocolo;
                arqBusca<<" || "<<packet1.vElementos[cont].tamanho<<" || "<<packet1.vElementos[cont].infomarcao<<endl<<endl;
                result = true;  // resultado positivo para a busca
            }else{
                if(result){  // verificar se acabou a sequencia de elementos iguais
                    vigia = false;
                }
            }
            cont++;
            if(cont == packet1.quantElements){  // Se terminar o packet, pula para o proximo
                if(packet1.nextPacket != -1){
                    this->lePacketArq(packet1, packet1.nextPacket);
                    cont=0;
                }else{
                    vigia = false;
                }if(not(result)){
                    vigia = false;
                }
            }
        }
    }else{  // demais casos de campos
        do{
            this->lePacketArq(packet1, packet1.nextPacket);
            // verifica os elementos do vetor de cada packet
            for(unsigned cont=0; cont<packet1.quantElements; cont++){
                if(this->comparaIgualdade(packet1.vElementos[cont], elementoBusca, campo)){  // Caso igual, escrve no arquivo de resultado
                    arqBusca<<"Packet: "<<packet1.id<<" Posicao: "<<cont<<endl;
                    arqBusca<<"Dados: "<<packet1.vElementos[cont].indice<<" || "<<packet1.vElementos[cont].tempo;
                    arqBusca<<" || "<<packet1.vElementos[cont].origem<<" || "<<packet1.vElementos[cont].destino<<" || "<<packet1.vElementos[cont].protocolo;
                    arqBusca<<" || "<<packet1.vElementos[cont].tamanho<<" || "<<packet1.vElementos[cont].infomarcao<<endl<<endl;
                    result = true;
                }
            }
        }while(packet1.nextPacket != -1);
    }
    clock_t fim = clock();

    if(!result){  // caso não encontrado
        arqBusca<<"Não encontrado (tempo: " << (fim - inicio)/(double) CLOCKS_PER_SEC << " segundos)" << endl << endl;
    }
    cout << "Arquivo de busca gerado com sucesso (tempo: " << (fim - inicio)/(double) CLOCKS_PER_SEC << " segundos)" << endl << endl;
    arqBusca.close();
}

//insiro um elemento
void SequenceSet::insere(bool casoInsercao){
    Packet packet1;
    pacote newPacote;
    string auxString;
    cout << "Criando novo pacote" << endl << endl;;
    cout << "-> Digite o indice: ";
    cin >> newPacote.indice;
    if(pegaElementoPorIndice(packet1, newPacote.indice) == -1){
        cout << "-> Digite o tempo: ";
        cin >> newPacote.tempo;
        cin.ignore();
        cout << "-> Digite o origem: ";
        getline(cin, auxString);
        strcpy(newPacote.origem, auxString.c_str());
        cout << "-> Digite o destino: ";
        getline(cin, auxString);
        strcpy(newPacote.destino, auxString.c_str());
        cout << "-> Digite o protocolo: ";
        getline(cin, auxString);
        strcpy(newPacote.protocolo, auxString.c_str());
        cout << "-> Digite o tamanho: ";
        cin >> newPacote.tamanho;
        cin.ignore();
        cout << "-> Digite a informacao: ";
        getline(cin, auxString);
        strcpy(newPacote.infomarcao, auxString.c_str());
        this->insereElemento(newPacote, casoInsercao);
        this->criaTxt();
        std::cout<<"Elemento inserido"<<std::endl;
    }else{
        cout << "Indice já existe! :(" << endl;
    }
}

// Set configuracoes iniciais do cabecalho
void SequenceSet::criaCabecalho(Cabecalho &cabecalho){
    cabecalho.quantPackets = 0;
    cabecalho.beginPacket = 0;
    cabecalho.endPacket = 0;
    cabecalho.nextPacket = 1;
    cabecalho.quantElements = 0;
};

// Set configuracoes iniciais do packet: recebe qual vai ser seu anterior, posterior e seu indice
void SequenceSet::criaPacket(Packet &packet, int later, unsigned id, int next){
    packet.id = id;
    packet.quantElements = 0;
    packet.nextPacket = next;
    packet.laterPacket = later;
};

// Insere um novo elemento no arquivo bin: recebe se vai utilizar (true), ou nao (false), a funcao "empresta"
void SequenceSet::insereElemento(pacote newPacote, bool casoEmpresta){
    Packet packet1;
    leCabecalhoArq();
    //faço a busca para saber em quem packet deve ser inserido
    int posPacket = this->arvore.busca(newPacote, -1, -1);
    if(posPacket == -1){//no caso de haver apenas um packet
        this->lePacketArq(packet1, this->cabecalho.beginPacket);
    }else{
        this->lePacketArq(packet1, posPacket);
    }
    insereNoPacket(packet1, newPacote, casoEmpresta);
    cabecalho.quantElements++;
    escreveCabecalhoArq();
}

// Compara se o primeiro parametro é menor (true), ou maior (false), que o segundo parametro
// (igualdade retorna false)
bool SequenceSet::compara(pacote& pacote1, pacote& pacote2, bool casoInsercao){
    int resposta = std::strcmp(pacote1.destino, pacote2.destino);
    if(resposta < 0){
        return true;
    }else if(resposta > 0){
        return false;
    }else if(casoInsercao){
        if(pacote1.indice >= pacote2.indice){
            return false;
        }else{
            return true;
        }
    }return false;
}

// Insere elemento no vetor de um packet especifico
void SequenceSet::insereNoVetor(pacote& newPacote, Packet& packet1){
    pacote aux;
    unsigned pos = packet1.quantElements; //comeca logo depois do ultimo elemento

    packet1.vElementos[pos] = newPacote;
    // Passa um elemento para frente ate achar sua posicao, com base no "compara"
    while(pos > 0){
        if(not(compara(packet1.vElementos[pos-1], packet1.vElementos[pos], true))){
            aux = packet1.vElementos[pos-1];
            packet1.vElementos[pos-1] = packet1.vElementos[pos];
            packet1.vElementos[pos] = aux;
            pos--;
        }else pos = 0;
    }
    packet1.quantElements++;

    escrevePacketArq(packet1, packet1.id);
}

// Divide os elementos do packet cheio com outro novo (ou vazio)
void SequenceSet::dividePacket(Packet& packet1, Packet& packet2){
    unsigned qtd = packet1.quantElements;
    Packet packetAux;
    //crio um packet2 novo, apartir de informações do cabecalho
    this->criaPacket(packet2, packet1.id, this->cabecalho.nextPacket, packet1.nextPacket);
    this->cabecalho.quantPackets++;
    this->cabecalho.nextPacket = this->cabecalho.quantPackets;
    packet1.nextPacket = packet2.id;
    
    // transfere metade dos elementos para o outro packet
    for(unsigned contPacket1 = (qtd/2); contPacket1 < qtd; contPacket1++){
        this->insereNoVetor(packet1.vElementos[contPacket1], packet2);
        packet1.quantElements--;
    }
    // atualiza a referencia ao ultimo packet (se necessario)
    if(this->cabecalho.endPacket == packet1.id){
        this->cabecalho.endPacket = packet2.id;
    }
    // atualiza a referencia ao anterior, do packet posterior
    if(packet2.nextPacket != -1){
        lePacketArq(packetAux, packet2.nextPacket);
        packetAux.laterPacket = packet2.id;
        escrevePacketArq(packetAux, packetAux.id);
        this->arvore.atualizaIndiceArvore(packetAux.vElementos[0], packet2.id);
    }
    escrevePacketArq(packet1, packet1.id);
    escrevePacketArq(packet2, packet2.id);

}

// Passa um elemento para o packet posterior ou anterior
void SequenceSet::empresta(Packet& packet1, Packet& packet2, bool qualPacket){
    if(qualPacket){  // packet posterior
        this->insereNoVetor(packet1.vElementos[packet1.quantElements-1], packet2);
        packet1.quantElements--;
    }else{  // packet anterior
        this->insereNoVetor(packet1.vElementos[0], packet2);
        for(unsigned cont = 0; cont < packet1.quantElements-1; cont++){  // retorna os elementos em uma posicao
            packet1.vElementos[cont] = packet1.vElementos[cont+1];
        }packet1.quantElements--;
    }
}

// Insere um elemento em um packet especifico: recebe se o caso é com (true) ou sem (false) a funcao "empresta"
void SequenceSet::insereNoPacket(Packet& packet1, pacote& newPacote, bool casoEmpresta){
    Packet packet2;
    bool control = false;

    if(packet1.quantElements < MAXelements){  // caso o packet tiver espaço
        insereNoVetor(newPacote, packet1);
    }else if(casoEmpresta){	// insercao com funcao empresta
        // empresta com o packet anterior
        if(packet1.laterPacket != -1){
            lePacketArq(packet2, packet1.laterPacket);

            if(packet2.quantElements < MAXelements){  // caso houver espaço
                // compara novamente qual packet o elemento deve ser inserido:
                // caso o primeiro elemento packet for menor que o novo elemento
                // passa um elemento ao anterior e insere no packet atual,
                // caso contrario insere direto no anterior
                empresta(packet1, packet2, 0);
                insereNoVetor(newPacote, packet1);
                this->arvore.atualizaPacoteArvore(packet2.vElementos[packet2.quantElements-1], packet1.vElementos[0]);
                control = true;
            }
        }
        // empresta com o packet posterior
        if(packet1.nextPacket != -1 and control == false){
            lePacketArq(packet2, packet1.nextPacket);
            if(packet2.quantElements < MAXelements){  // caso houver espaço
                // passo um elemento para o posterior e insiro no atual
                if(this->compara(packet1.vElementos[packet1.quantElements-1], newPacote, true)){
                    insereNoVetor(newPacote, packet2);
                }
                else{
                    empresta(packet1, packet2, 1);
                    insereNoVetor(newPacote, packet1);
                }
                this->arvore.atualizaPacoteArvore(packet2.vElementos[1], packet2.vElementos[0]);
                control = true;
            }
        }
        // realiza divide quando posterior e anterior estiverem cheios
        if(control == false){
            dividePacket(packet1, packet2);
            // verifica novamente em qual packet vai ser inserido
            if(not(compara(packet2.vElementos[0], newPacote, true))){
                insereNoVetor(newPacote, packet1);
            }else{
                insereNoVetor(newPacote, packet2);
            }this->arvore.promove(packet2.vElementos[0], packet1.id, packet2.id);
        }
    }else{  // insercao sem empresta
        dividePacket(packet1, packet2);
        // verifica novamente em qual packet vai ser inserido
        if(not(compara(packet2.vElementos[0], newPacote, true))){
            insereNoVetor(newPacote, packet1);
        }else{
            insereNoVetor(newPacote, packet2);
        }this->arvore.promove(packet2.vElementos[0], packet1.id, packet2.id);
    }
}

/*
Realiza comparacao da igualdade, com base em um campo especifico,
de dois elementos

Relacao valor de campo e qual campo do pacote:
campo 1 = indice
campo 2 = tempo;
campo 3 = origem;
campo 4 = destino;
campo 5 = protocolo;
campo 6 = tamanho;
campo 7 = informacao;
*/
bool SequenceSet::comparaIgualdade(pacote elemento1, pacote elemento2, int campo){
    if(campo==1){
        if(elemento1.indice == elemento2.indice){
            return true;
        }return false;
    }else if(campo==2){
        if(elemento1.tempo == elemento2.tempo){
            return true;
        }return false;
    }else if(campo==3){
        if(std::strcmp(elemento1.origem, elemento2.origem) == 0){
            return true;
        }return false;
    }else if(campo==4){
        if(std::strcmp(elemento1.destino, elemento2.destino) == 0){
            return true;
        }return false;
    }else if(campo==5){
        if(std::strcmp(elemento1.protocolo, elemento2.protocolo) == 0){
            return true;
        }return false;
    }else if(campo==6){
        if(elemento1.tamanho==elemento2.tamanho){
            return true;
        }return false;
    }else{
        if(std::strcmp(elemento1.infomarcao, elemento2.infomarcao) == 0){
            return true;
        }return false;
    }
}

// Pega o elemento pelo indice dele
int SequenceSet::pegaElementoPorIndice(Packet& packet1,int indiceBusca){
    leCabecalhoArq();
    pacote elemento;
    elemento.indice = indiceBusca;
    packet1.nextPacket = this->cabecalho.beginPacket;  // pega o primeiro packet
    do{
        this->lePacketArq(packet1, packet1.nextPacket);
        unsigned cont = 0;
        while(cont<packet1.quantElements){
            // Caso for igual, retorna a posicao no vetor e o packet (por referencia)
            if(this->comparaIgualdade(packet1.vElementos[cont], elemento, 1)){
                return cont;
            }
            cont++;
        }
    }while(packet1.nextPacket != -1);
    return -1;
}

// Cria arquivo txt com os dados do bin
void SequenceSet::criaTxt(){
    Packet packet;
    ofstream arqTxt("arq_comDados.txt", ios::out);
    if (not(arqTxt)) {
        throw runtime_error("*******Erro ao criar o arquivo txt (criaTXT)*******");
    }

    leCabecalhoArq();
    arqTxt << "Quantidade de Packets: " << cabecalho.quantPackets << endl;
    arqTxt << "Primeiro Packet: " << cabecalho.beginPacket << endl;
    arqTxt << "Ultimo Packet: " << cabecalho.endPacket << endl;
    arqTxt << "Proximo Packet: " << cabecalho.nextPacket << endl;
    arqTxt << "Quantidade de Elementos: " << cabecalho.quantElements << endl << endl;

    packet.nextPacket = cabecalho.beginPacket;

    do{
        lePacketArq(packet, packet.nextPacket);
        arqTxt << "ID: " << packet.id << endl;
        arqTxt << "Quantidade de elementos: " << packet.quantElements << endl;
        arqTxt << "Proximo Packet: " << packet.nextPacket << endl;
        arqTxt << "Packet Anterior: " << packet.laterPacket << endl;
        arqTxt << "Elementos:" << endl;
        for(unsigned c=0; c<packet.quantElements; c++){
            arqTxt << "Elemento " << c << ": ";
            arqTxt << packet.vElementos[c].indice << " || ";
            arqTxt << packet.vElementos[c].tempo << " || ";
            arqTxt << packet.vElementos[c].origem << " || ";
            arqTxt << packet.vElementos[c].destino << " || ";
            arqTxt << packet.vElementos[c].protocolo << " || ";
            arqTxt << packet.vElementos[c].tamanho << " || ";
            arqTxt << packet.vElementos[c].infomarcao << endl;
        }
        arqTxt << endl;
    }while(packet.nextPacket != -1);
}

// FUNCOES PARA ESCREVER E LER O ARQUIVO BIN

// Le packet do arquivo, com base no indice do packet
void SequenceSet::lePacketArq(Packet& packet, unsigned pos){
    fstream arqBinario("arq_comDados.bin", ios::in | ios::out | ios::binary);
    if (not(arqBinario)) {
        throw runtime_error("Erro ao criar o arquivo (leBloco)");
    }

    arqBinario.seekg(sizeof(Cabecalho)+sizeof(Packet)*pos, ios::beg);
    arqBinario.read(reinterpret_cast<char*> (&packet), sizeof(Packet));
    arqBinario.close();
}

// Escreve packet do arquivo, com base no indice do packet
void SequenceSet::escrevePacketArq(Packet& packet, unsigned pos){
    fstream arqBinario("arq_comDados.bin", ios::in | ios::out | ios::binary);
    if (not(arqBinario)) {
        throw runtime_error("Erro ao criar o arquivo (escreveBloco)");
    }

    arqBinario.seekp(sizeof(Cabecalho)+sizeof(Packet)*pos, ios::beg);
    arqBinario.write(reinterpret_cast<const char*> (&packet), sizeof(Packet));
    arqBinario.close();
}

// Le cabecalho do arquivo
void SequenceSet::leCabecalhoArq(){
    fstream arqBinario("arq_comDados.bin", ios::in | ios::out | ios::binary);
    if (not(arqBinario)) {
        throw runtime_error("Erro ao criar o arquivo (leCabecalho)");
    }

    arqBinario.seekg(0, ios::beg);
    arqBinario.read(reinterpret_cast<char*> (&cabecalho), sizeof(Cabecalho));
    arqBinario.close();
}

// Escreve cabecalho
void SequenceSet::escreveCabecalhoArq(){
    fstream arqBinario("arq_comDados.bin", ios::in | ios::out | ios::binary);
    if (not(arqBinario)) {
        throw runtime_error("Erro ao criar o arquivo (escreveCabecalho)");
    }

    arqBinario.seekp(0, ios::beg);
    arqBinario.write(reinterpret_cast<const char*> (&cabecalho), sizeof(Cabecalho));
    arqBinario.close();
}
