/*
Trabalho de Estrutura de Dados - GCC216
Ordenacao em Memoria Secundaria, com Sequence Set e Arvore B+
Copyright 2020 by Chrystian Arriel Amaral, Debora Rossini Martins Cardoso e Thiago Salles Santos
Arquivo arvoreBplus.h: definição e funções da class arvoreBplus que armazena chaves e indices do sequence set,
realizando busca de packets, com base em uma chave. Versão de arvore em arquivo.
*/

class arvoreBplus{
    private:
        // Maximo de chaves por nohs
        int MAXElements;
        // Estrutura do cabecalho da arvore
        struct cabecalhoArvore{
            int raiz;
            unsigned quantNohs;
            unsigned nextNoh;
            unsigned quantChaves;
        };
        // Estrutura de chaves
        struct Chaves{
            char chave1[40];
            int chave2;
        };
        // Estrutura do noh
        struct noh{
            Chaves vChave[80];
            unsigned nohFilho[81];
            int pai;
            bool folha;
            int id;
            int quantChaves;
        };
        // Variavel padrao para o cabecalho
        cabecalhoArvore cabecalho;
        // Seta as configurações iniciais do cabecalho
        void criaCabecalho();
        // Seta as configurações iniciais do noh
        void criaNoh(noh& noh1, int pai, int id);
        // Le um noh no arquivo da arvore
        void leNohArq(noh& noh1, int pos);
        // Escreve um noh no arquivo da arvore
        void escreveNohArq(noh& noh1, int pos);
        // Le um cabecalho no arquivo de arvore
        void leCabecalhoArq();
        // Escreve um cabecalho no arquivo da arvore
        void escreveCabecalhoArq();
        // Seta uma estrutura de chaves com as chaves de um pacote
        void criaChave(Chaves& chave, pacote pacote1);
        // Compara duas chaves
        int comparaChaves(Chaves chaveA, Chaves chaveB);
        // Busca a posição de uma chave dentro de um noh
        int buscaNoNoh(noh& noh1, Chaves chave);
        // Busca uma chave pela arvore
        int buscaNaArvore(noh& noh1, Chaves chaves);
        // Insere uma chave e seus indices em um noh
        void insereNoNoh(noh& noh1, Chaves chave, unsigned idAnterior, unsigned idPosterior);
        // Divide um noh cheio, criando um novo e passando metade dos elementos
        void divideNoh(noh& nohAntigo, noh& nohNovo, Chaves chaveNova, unsigned idAnterior, unsigned idPosterior);
    public:
        // Construtor
        arvoreBplus();
        // Promove um pacote do sequence set para a arvore
        void promove(pacote pacoteNovo, unsigned idAntigo, unsigned idPosterior);
        // Atualiza uma chave existente por outra nova, sem mudar os indices
        void atualizaChave(pacote pacoteAntigo, pacote pacoteNovo);
        // Busca um packet ideal para um elemento do sequence set
        int buscaPacket(pacote umPacote);
        // Cria Txt do arquivo da arvore
        void criaTxt();
        // Verifica se existe arquivo de arvore
        bool testeDeArquivo();
        // Seta as configurações iniciais da arvore
        void setInicial();
};

arvoreBplus::arvoreBplus(){
    MAXElements = 80;
}

// Verifica se existe arquivo de arvore
bool arvoreBplus::testeDeArquivo(){
    fstream arq("arq_comArvore.bin", ios::in | ios::out | ios::binary);
    if (arq) {
        arq.close();
        return true;  // se houver arquivo
    }
    arq.close();
    return false;
}

// Seta as configurações iniciais da arvore
void arvoreBplus::setInicial(){
    noh noh1;
    fstream arqNovo("arq_comArvore.bin", ios::in | ios::out | ios::binary | ios::trunc);
    if(not(arqNovo)){
        throw runtime_error("Erro ao criar o arquivo bin (Arquivo com arvore)");
    }
    arqNovo.close();
    
    criaCabecalho();
    criaNoh(noh1, -1, 0); // cria um noh vazio inicial
    escreveNohArq(noh1, 0);
    cabecalho.quantNohs++;
    escreveCabecalhoArq();
}

// Compara duas chaves A e B, pela primeira e segunda chave, retornando:
// retorna = -1 (A < B)
// retorna = 0 (A == B)
// retorna = 1 (A > B)
int arvoreBplus::comparaChaves(Chaves chaveA, Chaves chaveB){
    int resposta = std::strcmp(chaveA.chave1, chaveB.chave1);
    if(resposta == 0){
        if(chaveA.chave2 > chaveB.chave2){
            return 1;
        }else if(chaveA.chave2 == chaveB.chave2){
            return 0;
        }else{
            return -1;
        }
    }return resposta;
}

// Busca uma chave dentro de um noh, retornando sua posição
int arvoreBplus::buscaNoNoh(noh& noh1, Chaves chave){
    for(int c=0; c<noh1.quantChaves; c++){
        if(comparaChaves(noh1.vChave[c], chave) >= 0){
            return c;
        }
    }
    return -1; // Caso não encontre a chave
}

// Procura um packet ideial para um pacote passado pelo sequence set
int arvoreBplus::buscaPacket(pacote umPacote){
    Chaves chave;
    criaChave(chave, umPacote);
    noh noh1;
    leCabecalhoArq();
    
    if(cabecalho.quantChaves == 0){ // Caso ainda não houver chaves
        return 0;
    }
    
    int pos = buscaNaArvore(noh1, chave);
    
    if(pos == -1){ 
        // Caso não encontre dentro do ultimo noh,
        // retorna o indice mais a direita do vetor
        return noh1.nohFilho[noh1.quantChaves];
    }
    return noh1.nohFilho[pos]; // retorna o indice da esquerda correspondente
}

// Busca uma chave na arvore, pulando de noh em noh ate achar uma folha
int arvoreBplus::buscaNaArvore(noh& noh1, Chaves chave){
    int pos;
    leNohArq(noh1, cabecalho.raiz);
    while(noh1.folha != true){
        if(noh1.folha != true){ // Caso nao for folha
            pos = buscaNoNoh(noh1, chave);
            if(pos == -1){ // Vai para noh mais a direita do vetor
                leNohArq(noh1, noh1.nohFilho[noh1.quantChaves]);
            }else{
                // Vai para noh a esquerda correspondente
                leNohArq(noh1, noh1.nohFilho[pos]);
            }
        }
    }
    pos = buscaNoNoh(noh1, chave);
    
    return pos;
}

// Recebe um elemento do sequence set para ser "promovido" na arvore.
// recebe o elemento e os indices do packet dele e anterior a ele.
void arvoreBplus::promove(pacote pacoteNovo, unsigned idAnterior, unsigned idPosterior){
    leCabecalhoArq();
    Chaves chaveNova;
    noh nohAux;
    criaChave(chaveNova, pacoteNovo);
    buscaNaArvore(nohAux, chaveNova);
    
    if(nohAux.quantChaves < MAXElements){ // Caso não estiver cheio o noh
        insereNoNoh(nohAux, chaveNova, idAnterior, idPosterior);
    }else{ // Caso estiver cheio, divide
        noh nohNovo;
        divideNoh(nohAux, nohNovo, chaveNova, idAnterior, idPosterior);
    }
    
    cabecalho.quantChaves++;
    escreveCabecalhoArq();
}

// Insere uma nova chave em um noh e desloca as demais ja existentes
void arvoreBplus::insereNoNoh(noh& noh1, Chaves chave, unsigned idAnterior, unsigned idPosterior){
    int pos = buscaNoNoh(noh1, chave); // Acha a posição para inserir
    
    if(pos == -1){ // Caso ela seja maior que todas, insere no final
        noh1.vChave[noh1.quantChaves] = chave;
        noh1.nohFilho[noh1.quantChaves] = idAnterior;
        noh1.nohFilho[noh1.quantChaves+1] = idPosterior;
    }else{
        for(int c=noh1.quantChaves; c>pos; c--){ // Desloca todas uma posição
            noh1.vChave[c] = noh1.vChave[c-1];
            noh1.nohFilho[c+1] = noh1.nohFilho[c];
            noh1.nohFilho[c] = noh1.nohFilho[c-1];
        }
        noh1.vChave[pos] = chave; // Insere a chave e os indices na posição
        noh1.nohFilho[pos] = idAnterior;
        noh1.nohFilho[pos+1] = idPosterior;
    }
    noh1.quantChaves++;
    escreveNohArq(noh1, noh1.id);
}

// Atualiza uma chave da arvore, não alterando seus indices
void arvoreBplus::atualizaChave(pacote pacoteAntigo, pacote pacoteNovo){
    leCabecalhoArq();
    Chaves chaveAntiga;
    Chaves chaveNova;
    noh nohAux;
    criaChave(chaveAntiga, pacoteAntigo);
    criaChave(chaveNova, pacoteNovo);
    int pos = buscaNaArvore(nohAux, chaveAntiga); // Procura chave a ser atualizada
    
    if(comparaChaves(nohAux.vChave[pos], chaveAntiga) == 0){ // Atualiza e reescreve no arquivo
        nohAux.vChave[pos] = chaveNova;
        escreveNohArq(nohAux, nohAux.id);
    }
    escreveCabecalhoArq();
}

// Divide um noh e cria um novo, passando metade dos elementos do cheio
void arvoreBplus::divideNoh(noh& nohAntigo, noh& nohNovo, Chaves chaveNova, unsigned idAnterior, unsigned idPosterior){
    unsigned qtd = nohAntigo.quantChaves;
    
    this->criaNoh(nohNovo, nohAntigo.pai, cabecalho.nextNoh); // Cria novo noh
    this->cabecalho.quantNohs++;
    this->cabecalho.nextNoh = this->cabecalho.quantNohs;
    
    if(nohAntigo.folha == false){ // Caso estiver em um nivel intermediario da arvore
        nohNovo.folha = false;
    }
    
    //transfere metade dos elementos para o outro noh
    for(unsigned c = (qtd/2); c < qtd; c++){
        this->insereNoNoh(nohNovo, nohAntigo.vChave[c], nohAntigo.nohFilho[c], nohAntigo.nohFilho[c+1]);
        nohAntigo.quantChaves--;
    }
    
    if(cabecalho.raiz == nohAntigo.id){ // Caso seja a raiz, cria nova raiz 
        noh novaRaiz;
        this->criaNoh(novaRaiz, -1, cabecalho.nextNoh);
        this->cabecalho.quantNohs++;
        this->cabecalho.nextNoh = this->cabecalho.quantNohs;
        
        novaRaiz.folha= false;
        // Atualiza pai dos nohs e raiz do cabecalho
        nohAntigo.pai = novaRaiz.id; 
        nohNovo.pai = novaRaiz.id;
        cabecalho.raiz = novaRaiz.id;
        escreveNohArq(novaRaiz, novaRaiz.id);
    }
    
    // Caso a nova chave deva ir para o noh antigo, insere nele
    // e passa a ultima para a nova, para que o noh não perca o nivel minimo
    if(comparaChaves(nohNovo.vChave[0], chaveNova) >= 0){
        insereNoNoh(nohAntigo, chaveNova, idAnterior, idPosterior);
        insereNoNoh(nohNovo, nohAntigo.vChave[nohAntigo.quantChaves-1], nohAntigo.nohFilho[nohAntigo.quantChaves-1], nohAntigo.nohFilho[nohAntigo.quantChaves]);
        nohAntigo.quantChaves--;
    }else{
        // Caso seja para inserir no noh novo, somente insere
        insereNoNoh(nohNovo, chaveNova, idAnterior, idPosterior);
    }
    noh pai;
    
    leNohArq(pai, nohNovo.pai);
    // Promove primeira chave do novo noh para o pai
    if(pai.quantChaves < MAXElements){
        insereNoNoh(pai, nohNovo.vChave[0], nohAntigo.id, nohNovo.id);
    }else{
        // Divide se o pai caso estiver cheio
        noh novo;
        divideNoh(pai, novo, nohNovo.vChave[0], nohAntigo.id, nohNovo.id);
    }
    escreveNohArq(pai, pai.id);
    
    // Retira elemento promovido do novo noh
    for(int c=0; c<nohNovo.quantChaves-1; c++){
        nohNovo.vChave[c] = nohNovo.vChave[c+1];
        nohNovo.nohFilho[c] = nohNovo.nohFilho[c+1];
        nohNovo.nohFilho[c+1] = nohNovo.nohFilho[c+2];
    }
    nohNovo.quantChaves--;
    
    escreveNohArq(nohAntigo, nohAntigo.id);
    escreveNohArq(nohNovo, nohNovo.id);
}

// Cria Txt com os nohs e o cabecalho da arvore presentes no arquivo
// Não fica em ordem, somente breve exibição dos dados da arvore
void arvoreBplus::criaTxt(){
    noh noh1;
    ofstream arqTxt("arq_comArvore.txt", ios::out);
    if (not(arqTxt)) {
        throw runtime_error("*******Erro ao criar o arquivo da arvore txt (criaTXT)*******");
    }

    leCabecalhoArq();
    arqTxt << "Quantidade de Nos: " << cabecalho.quantNohs << endl;
    arqTxt << "Raiz: " << cabecalho.raiz << endl;
    arqTxt << "Proximo Noh: " << cabecalho.nextNoh << endl;
    arqTxt << "Quantidade de Chaves: " << cabecalho.quantChaves << endl << endl;

    for(unsigned d=0; d<cabecalho.quantNohs; d++){
        leNohArq(noh1, d);
        arqTxt << "-----------------------------------------------------------" << endl;
        arqTxt << "Noh " << noh1.id << endl;
        arqTxt << "Quantidade de Chaves: " << noh1.quantChaves << endl;
        if(noh1.folha == true){
            arqTxt << "Noh Folha" << endl;
        }
        arqTxt << "Noh pai: " << noh1.pai << endl;
        arqTxt << "Chaves: " << endl;
        for(int c=0; c<noh1.quantChaves; c++){
            arqTxt << "Chave 1: " << noh1.vChave[c].chave1 << " || ";
            arqTxt << "Chave 2: " << noh1.vChave[c].chave2 << " || ";
            arqTxt << "nohFilho 1: " << noh1.nohFilho[c] << " || ";
            arqTxt << "nohFilho 2: " << noh1.nohFilho[c+1] << endl;
        }
    }
}

//FUNCOES PARA SETAR ESTRUTURAS

// Set as chaves na estrutura
void arvoreBplus::criaChave(Chaves& chave, pacote pacote1){
    chave.chave2 = pacote1.indice;
    size_t size = sizeof(pacote1.destino);
    strncpy(chave.chave1, pacote1.destino, size);
}

// Set configuracoes iniciais do cabecalho
void arvoreBplus::criaCabecalho(){
    cabecalho.quantNohs = 0;
    cabecalho.raiz = 0;
    cabecalho.nextNoh = 1;
    cabecalho.quantChaves = 0;
};

// Set configuracoes iniciais do noh: recebe qual vai ser seu pai, filhos e seu indice
void arvoreBplus::criaNoh(noh& noh1, int pai, int id){
    noh1.folha = true;
    noh1.id = id;
    noh1.pai = pai;
    noh1.quantChaves = 0;
};

// FUNCOES PARA ESCREVER E LER O ARQUIVO BIN

// Le packet do arquivo, com base no indice do packet
void arvoreBplus::leNohArq(noh& noh1, int pos){
    fstream arqBinario("arq_comArvore.bin", ios::in | ios::out | ios::binary);
    if (not(arqBinario)) {
        throw runtime_error("Erro ao criar o arquivo (leNoh)");
    }

    arqBinario.seekg(sizeof(cabecalhoArvore)+sizeof(noh)*pos, ios::beg);
    arqBinario.read(reinterpret_cast<char*> (&noh1), sizeof(noh));
    arqBinario.close();
}

// Escreve packet do arquivo, com base no indice do packet
void arvoreBplus::escreveNohArq(noh& noh1, int pos){
    fstream arqBinario("arq_comArvore.bin", ios::in | ios::out | ios::binary);
    if (not(arqBinario)) {
        throw runtime_error("Erro ao criar o arquivo (escreveNoh)");
    }

    arqBinario.seekp(sizeof(cabecalhoArvore)+sizeof(noh)*pos, ios::beg);
    arqBinario.write(reinterpret_cast<const char*> (&noh1), sizeof(noh));
    arqBinario.close();
}

// Le cabecalho do arquivo
void arvoreBplus::leCabecalhoArq(){
    fstream arqBinario("arq_comArvore.bin", ios::in | ios::out | ios::binary);
    if (not(arqBinario)) {
        throw runtime_error("Erro ao criar o arquivo (leCabecalhoArvore)");
    }

    arqBinario.seekg(0, ios::beg);
    arqBinario.read(reinterpret_cast<char*> (&cabecalho), sizeof(cabecalhoArvore));
    arqBinario.close();
}

// Escreve cabecalho
void arvoreBplus::escreveCabecalhoArq(){
    fstream arqBinario("arq_comArvore.bin", ios::in | ios::out | ios::binary);
    if (not(arqBinario)) {
        throw runtime_error("Erro ao criar o arquivo (escreveCabecalhoArvore)");
    }

    arqBinario.seekp(0, ios::beg);
    arqBinario.write(reinterpret_cast<const char*> (&cabecalho), sizeof(cabecalhoArvore));
    arqBinario.close();
}
