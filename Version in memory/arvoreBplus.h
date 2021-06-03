/*
Trabalho de Estrutura de Dados - GCC216
Ordenacao em Memoria Secundaria, com Sequence Set e Arvore B+
Copyright 2020 by Chrystian Arriel Amaral, Debora Rossini Martins Cardoso e Thiago Salles Santos
Arquivo arvoreBplus.h: definição e funções da class arvoreBplus que armazena chaves e indices do sequence set,
realizando busca de packets, com base em uma chave. Versão de arvore em memoria. 
*/

class galho;//pre-declaro galho pois noh utiliza ela
class noh{
	//definição dos amigos da classe noh
    friend class arvoreBplus;
	friend class galho;
	private:
		char destino[40];///chave primaria para comparação
		int indice;//chave secundaria para comparação
		//ponterios para saber quem é seu antenrior e posterior na "Lista" (galho)
		noh* nohAnterior;
		noh* nohPosterior;
		//ponteiros de galhos 
		galho* galhoAnterior;//Anterior(baixo esquerda) menores q o elemento
		galho* galhoPosterior;//Posterior(baixo direita) maiores iguais ao elemento
		int anteriorPacket;//Indice do packet anterior ao elemento no SequenceSet (<)
		int indicePacket;//Indice do packet do eleemtno no SequenceSet (>=)
	public:
		noh(pacote pacote1, int indicePacket1, int indicePacket2);
		~noh();
		void atualizaIndiceAnteriorNoh(int indiceAnteriorNovo);//atualiza indice anterior
		void atualizaPacoteNoh(pacote pacote1);//atualiza destino e indice
};

//funcionará como uma "Lista"
class galho{
	//definição dos amigos da classe galho
	friend class arvoreBplus;
	private:
		noh* primeiroNoh;//variavel para saber onde começa o galho (menor elemento)
		galho* pai;//saber se o galho possui um pai
		int limiteNohs;//limite de nohs q ele pode ter
		int quantElements;//quantidade de nohs q ele está armazenando
	public:
		galho();
		~galho();
		int comparaNohs(noh* noh1, noh* noh2);//compara noh's e retorna de acordo com std::strcmp
		galho* insereNoGalho(noh* nohNovo);//insere um valor ao galho, retorna o galho pai. quando ocorre o divide
		galho* divideGalho();//divide o galho e retorna um novo com a metade direita dos dados do galho ao qual ele se originou
		galho* promoveNoh();//promove um elemento para o pai, se necessario cria um pai, e retorna o pai
		void atualizaPonteiros(noh* nohProcura);//atualiza ponteiros de galhos de um noh para quando ocorre uma divisão de galhos
};

class arvoreBplus{
	private:
		galho* raiz;//raiz da arvore
	public:
		arvoreBplus();
		void promove(pacote pacote1, int indicePacket1, int indicePacket2);//promove um noh a arvore q irá inserir no galho correspondente
		int	buscaAux(galho* galhoAux, noh* novoNoh);//busca o packet onde o valor deve ser inserido
		int busca(pacote pacote1, int indicePacket1, int indicePacket2);//busca o packet onde o valor deve ser inserido
		noh* atualizaAux(galho* galhoAux, noh* nohAntigo);//busca o noh q deve ser atualizado
		void atualizaPacoteArvore(pacote pacoteAntigo, pacote pacote1);//atualiza o pacote do noh
		void atualizaIndiceArvore(pacote pacoteAntigo, int indiceAnteriorPacketNovo);//atualiza o indice do noh
		galho* buscaGalho(galho* galhoAux, noh* novoNoh);//busca o galho onde o valor será promovido/inserido
};

//indicePacket1 é o indice do packet anterior
//indicePacket2 é o indice do packet onde pacote se encontra
noh::noh(pacote pacote1, int indicePacket1, int indicePacket2){
	this->indice = pacote1.indice;
	std::strcpy(this->destino, pacote1.destino);
	this->anteriorPacket = indicePacket1;
	this->indicePacket = indicePacket2;
	this->galhoAnterior = NULL;
	this->galhoPosterior = NULL;
	this->nohAnterior = NULL;
	this->nohPosterior = NULL;
}

noh::~noh(){
}

//atualiza as chaves 
void noh::atualizaPacoteNoh(pacote pacote1){
	this->indice = pacote1.indice;
	std::strcpy(this->destino, pacote1.destino);
}

//atualiza indice anterior
void noh::atualizaIndiceAnteriorNoh(int indiceAnteriorNovo){
	this->anteriorPacket = indiceAnteriorNovo;
}

//cria um galho com limite já definido de 80 nohs
galho::galho(){
	this->limiteNohs = 80;
	this->primeiroNoh = NULL;
	this->pai = NULL;
	this->quantElements = 0;
}

galho::~galho(){
}

//retornes da std::strcmp(a, b)
// retorna = <0 (a < b)
// retorna = 0 (a = b)
// retorna = >0 (a > b)
int galho::comparaNohs(noh* nohGalho, noh* nohNovo){
	int resposta = std::strcmp(nohGalho->destino, nohNovo->destino);//compara priemiro a chave primaria
	if(resposta == 0){//se a chave primaria for igual irá compara a secundaria
		if(nohGalho->indice < nohNovo->indice){
			return -1;
		}if(nohGalho->indice == nohNovo->indice){
			return 0;
		}if(nohGalho->indice > nohNovo->indice){
			return 1;
		}
	}return resposta;
}

//inserir um noh ao galho
galho* galho::insereNoGalho(noh* nohNovo){
	if(this->quantElements < this->limiteNohs){//verifica se a quantidade de elementos não estorou o limite
		if(this->primeiroNoh != NULL){//vejo se meu galho não está vazio
			noh* aux = this->primeiroNoh;//copio o primeiro noh
			bool vigia = true;
			while((this->comparaNohs(aux, nohNovo) <= 0) and (vigia)){//procuro onde devo inserir o noh
				if(aux->nohPosterior != NULL){
					aux = aux->nohPosterior;
				}else{
					vigia = false;
				}
			}
			//achado um noh como referencia vejo se insiro atras(anterior) ou na frente(posterior) do noh de referencia
			if(this->comparaNohs(aux, nohNovo) > 0){//anterior
				//faço a inserção do noh na posição átras ao noh de referencia
				nohNovo->nohAnterior = aux->nohAnterior;
				aux->nohAnterior = nohNovo;
				nohNovo->nohPosterior = aux;
				if(nohNovo->nohAnterior != NULL){//vejo se o nohnovo não se tornou o primeiro noh do galho
					nohNovo->nohAnterior->nohPosterior = nohNovo;//caso seja arrumo os ponteiros de quem está átras dele
				}else{//caso tenha se tornado o primeiro noh aponto o ponteiro primeiroNoh do galho para ele
					this->primeiroNoh = nohNovo;
				}
			}else{//posterior
				//faço a inserção do noh na posição a frente ao noh de referencia
				nohNovo->nohAnterior = aux;
				nohNovo->nohPosterior = aux->nohPosterior;
				aux->nohPosterior = nohNovo;
				if(nohNovo->nohPosterior != NULL){//verifico se o nohnovo não é último noh do galho
					nohNovo->nohPosterior->nohAnterior = nohNovo;//caso seja arrumo os ponteiro de quem está a frente dele
				}
			}
		}else{//caso vazio apenas aponto para o noh q acabou de ser criado
			this->primeiroNoh = nohNovo;
		}this->quantElements++;//aumenta a quantidade de nohs
		return NULL;//como não fez o divide não precisou retorna o pai
	}else{//caso em quem o galho está cheio
		galho* novoGalho = this->divideGalho(); //divido o galho
		//comparo onde devo inserir o nohnovo, se é galhoNovo criado pela divisão ou no galho "antigo", e insiro ele
		if(this->comparaNohs(novoGalho->primeiroNoh, nohNovo) <= 0){
			novoGalho->insereNoGalho(nohNovo);
		}else{
			this->insereNoGalho(nohNovo);
		}
		//arrumo os ponteiros de galhos anterior e posterior do elemento q será promovido ao galho pai 
		//elemento a ser promovido será o primeiroNoh do NovoGalho
		//aponto o anterior ao galho "antigo" (this)
		//aponto o psoterior ao novoGalho vindo da divisão (novoGalho)
		novoGalho->primeiroNoh->galhoAnterior = this;
		novoGalho->primeiroNoh->galhoPosterior = novoGalho;
		galho* paiGalho = novoGalho->promoveNoh();//promovo o elemento
		this->pai = paiGalho;//arrumo o ponteiro de pai do galho "antigo" (será sobrescrito caso ele já tenha um pai)
		return paiGalho;//retorno o pai, para ver se tornará a nova raiz
	}
}

//divido o galho
galho* galho::divideGalho(){
	galho* novoGalho = new galho();//crio galho novo para ser a metdade da direita do galho origem
	//vou até a metade dos nohs
	noh* aux = this->primeiroNoh;
	for(int c=0; c < (this->quantElements/2)-1; c++){
		aux = aux->nohPosterior;
	}
	novoGalho->primeiroNoh = aux->nohPosterior;//deifino q o priemiro noh do novo será a metade a deireita 
	//corto as "relações" entre os elementos do meio
	aux->nohPosterior = NULL;
	novoGalho->primeiroNoh->nohAnterior = NULL;
	this->quantElements = this->quantElements/2;//diminuo a quantidade de elemento pela metade
	novoGalho->quantElements = this->quantElements;
	novoGalho->pai = this->pai;//copio o pai do da origem para o novoGalho
	return novoGalho;//retorno o galho criado pela divisão
}

void galho::atualizaPonteiros(noh* nohProcura){
	//procuro o lugar do elemento recem inserido no pai para atualizar os ponteiros do seu posterior
    noh* nohAux = this->primeiroNoh;
    while(this->comparaNohs(nohAux, nohProcura) != 0){
        nohAux = nohAux->nohPosterior;
    }if(nohAux->nohPosterior != NULL){//vejo se elee tem um posterior
		//caso ele tenha um posterior tenho q atualizar seu ponteiro anterior, pois foi crado um galho entre seu antigo anterior e seu atual posterior
        nohAux = nohAux->nohPosterior;
        nohAux->galhoAnterior = nohProcura->galhoPosterior;
    }
}

galho* galho::promoveNoh(){
	galho* paiGalho;
	//vejo se o galho onde to possui um pai, caso não crio, caso sim uso o já existente
	if(this->pai == NULL){
		paiGalho = new galho();
	}else{
		paiGalho = this->pai;
	}
	//pego o primeironoh do galho onde eu to corto sua realações com o restantes dos nohs
	noh* aux = this->primeiroNoh;
	this->primeiroNoh = this->primeiroNoh->nohPosterior;
	this->primeiroNoh->nohAnterior = NULL;
	aux->nohPosterior = NULL;
	//depois de cortar sua relações com o galho, insiro ele no pai
	paiGalho->insereNoGalho(aux);
    paiGalho->atualizaPonteiros(aux);//atualizo os ponteiros
	this->pai = paiGalho;//arrumo o ponteiro de pai do galho onde estou(será sobrescrito caso ele já tenha um pai)
    return paiGalho;//retorno o pai
}

arvoreBplus::arvoreBplus(){
	this->raiz = NULL;
}

void arvoreBplus::promove(pacote pacote1, int indicePacket1, int indicePacket2){
	noh* novoNoh = new noh( pacote1, indicePacket1, indicePacket2);//crio um noh para inserção
	if(this->raiz == NULL){//vejo se possuo raiz
		galho* novoGalho = new galho();//crio um galho
		novoGalho->insereNoGalho(novoNoh);//insiro no novoGalho o noh
		this->raiz = novoGalho;//defino minha nova raiz
	}else{
		galho* novoGalho = this->raiz;//faço uma copia da minha raiz
		novoGalho = this->buscaGalho(novoGalho, novoNoh);//busco em que galho devo inserir o nohNovo
		galho* paiGalho = novoGalho->insereNoGalho(novoNoh);//insiro o nohNovo, recebo se um pai foi utilizado
		if((paiGalho != NULL) and (paiGalho->pai == NULL)){//caso tenha utilizado um pai durante a inserção no galho, vejo se ele tem um pai, caso não tenha será minha raiz
			this->raiz = paiGalho;
		}
	}
}

galho* arvoreBplus::buscaGalho(galho* galhoAux, noh* novoNoh){
	noh* nohAux = galhoAux->primeiroNoh;
	//procuro o elemento, vejo se ele é último caso seja o último possui um tratamento especial
	while(nohAux->nohPosterior != NULL){//laço para ver se é o último, e até lá se não achar o seu local antes
		int resposta = galhoAux->comparaNohs(nohAux, novoNoh);//pego a respota da compação dos nohs
		if(resposta == 0){//caso de elemento igual(impossivel de acontecer)
			if(nohAux->galhoPosterior != NULL){//vejo se o elemento possui galho a sua direita baixo, se tiver vou para lá e retorno de lá
				galhoAux = nohAux->galhoPosterior;
				return this->buscaGalho(galhoAux, novoNoh);
			}else{//caso não tenha retorno o galho ao qual me encontro
				return galhoAux;
			}
		}else if(resposta < 0){//caso valor q procuro seja maior do noh do galho que estou vou mais para a direita
			nohAux = nohAux->nohPosterior;
        }else{//caso valor q procuro seja menor do noh do galho que estou
			if(nohAux->galhoAnterior != NULL){//vejo se tem galho a sua esquerda baixo, caso tenha vou para lá e retorno de lá
				galhoAux = nohAux->galhoAnterior;
				return this->buscaGalho(galhoAux, novoNoh);
			}else{//caso não retorno o galho ao qual me encontro
				return galhoAux;
			}
		}
	}//caso especial do último noh
	int resposta = galhoAux->comparaNohs(nohAux, novoNoh);//pego a respota da compação dos nohs
	if(resposta <= 0){//caso elemento seja maior igual 
		if(nohAux->galhoPosterior != NULL){//vejo se posso descer a direita baixo, caso se vou até lá e retorno por lá
			galhoAux = nohAux->galhoPosterior;
			return this->buscaGalho(galhoAux, novoNoh);
		}else{//caso não retorno o galho ao qual me encontro
			return galhoAux;
		}
	}else{//caso seja menor
		if(nohAux->galhoAnterior != NULL){//vejo se tem galho a sua esquerda baixo, caso tenha vou para lá e retorno de lá
			galhoAux = nohAux->galhoAnterior;
			return this->buscaGalho(galhoAux, novoNoh);
		}else{//caso não retorno o galho ao qual me encontro
			return galhoAux;
		}
	}
}

int arvoreBplus::buscaAux(galho* galhoAux, noh* novoNoh){
	noh* nohAux = galhoAux->primeiroNoh;
	//procuro o elemento, vejo se ele é último caso seja o último possui um tratamento especial
	while(nohAux->nohPosterior != NULL){//laço para ver se é o último, e até lá se não achar o seu local antes
		int resposta = galhoAux->comparaNohs(nohAux, novoNoh);//pego a resposta da comparação entre o elemento a ser inserido no sequence set com noh dentro do galho
		if(resposta == 0){//caso seja igual retorno o indice do Packet que meu noh da arvore faz parte no sequenceset
			return nohAux->indicePacket;
		}if(resposta < 0){//caso elemento a ser inserido(novoNoh) seja maior continuo a ir a direita do galho
			nohAux = nohAux->nohPosterior;
		}else{//caso elemento a ser inserido(novoNoh) seja menor
			if(nohAux->galhoAnterior != NULL){//vejo se posso descer a direita, caso sim irei descer a esquerda do noh e retornar o valor obtido por lá
				galhoAux = nohAux->galhoAnterior;
				return this->buscaAux(galhoAux, novoNoh);
			}else{//caso não, retorno o indice do packet anterior que meu noh da arvore faz parte no sequenceset
				return nohAux->anteriorPacket;
			}
		}
	}//caso especial do último noh do galho
	int resposta = galhoAux->comparaNohs(nohAux, novoNoh);//pego a resposta da comparação de nohs
	if(resposta == 0){//caso seja igual retorno o indice do Packet que meu noh da arvore faz parte no sequenceset
		return nohAux->indicePacket;
	}
	if(resposta < 0){//caso elemento a ser inserido(novoNoh) seja maior continuo a ir a direita do galho
		if(nohAux->galhoPosterior != NULL){//vejo se posso ir a direita baixo, caso sim irei descer a direita do noh e retornar o valor obtido por lá
			galhoAux = nohAux->galhoPosterior;
			return this->buscaAux(galhoAux, novoNoh);
		}else{//caso seja igual retorno o indice do Packet que meu noh da arvore faz parte no sequenceset
			return nohAux->indicePacket;
		}
	}else{//caso elemento a ser inserido(novoNoh) seja menor
		if(nohAux->galhoAnterior != NULL){//vejo se posso descer a direita, caso sim irei descer a esquerda do noh e retornar o valor obtido por lá
			galhoAux = nohAux->galhoAnterior;
			return this->buscaAux(galhoAux, novoNoh);
		}else{//caso não, retorno o indice do packet anterior que meu noh da arvore faz parte no sequenceset
			return nohAux->anteriorPacket;
		}
	}
}

//busca chama o busca aux passando um galho como inicio da busca recursiva, q no caso é galho da raiz o inicio da busca
int arvoreBplus::busca(pacote pacote1, int indicePacket1, int indicePacket2){
	if(this->raiz == NULL){//caso não tenha ocorrido a primeira divisão
		return -1;
	}noh* novoNoh = new noh(pacote1, indicePacket1, indicePacket2);
	galho* galhoAux = this->raiz;
	return this->buscaAux(galhoAux, novoNoh);
}

//busca um noh na arvore
noh* arvoreBplus::atualizaAux(galho* galhoAux, noh* nohAntigo){
	noh* nohAux = galhoAux->primeiroNoh;
	//procuro o elemento, vejo se ele é último caso seja o último possui um tratamento especial
	while(nohAux->nohPosterior != NULL){//laço para ver se é o último, e até lá se não achar o seu local antes
		int resposta = galhoAux->comparaNohs(nohAux, nohAntigo);//pego a resposta da comparação entre os nohs
		if(resposta == 0){//caso igual retorno o noh desejado
			return nohAux;
		}else {
			if(resposta < 0){//caso noh q procuro seja maior do que me encontro no galho, continuo indo a direita do galho
				nohAux = nohAux->nohPosterior;
			}else{//caso noh q procuro seja menos  do que me encontro no galho
				if(nohAux->galhoAnterior != NULL){//verifico se posso descer a esquerda baixo do noh, caso sim desço e retorno o valor obtido por lá
					galhoAux = nohAux->galhoAnterior;
					return this->atualizaAux(galhoAux, nohAntigo);
				}return NULL;//caso não retorna NULL, que será "erro"
			}
		}
	}int resposta = galhoAux->comparaNohs(nohAux, nohAntigo);//pego a resposta da comparação entre os nohs
	if(resposta == 0){//caso igual retorno o noh desejado
		return nohAux;
	}else{
		if(resposta < 0){//caso noh q procuro seja maior do que me encontro no galho, continuo indo a direita do galho
			if(nohAux->galhoPosterior != NULL){//verifico se posso descer a direita baixo do noh, caso sim desço e retorno o valor obtido por lá
				galhoAux = nohAux->galhoPosterior;
				return this->atualizaAux(galhoAux, nohAntigo);
			}return NULL;//caso não retorna NULL, que será "erro"
		}else{//caso noh q procuro seja menos  do que me encontro no galho
			if(nohAux->galhoAnterior != NULL){//verifico se posso descer a esquerda baixo do noh, caso sim desço e retorno o valor obtido por lá
				galhoAux = nohAux->galhoAnterior;
				return this->atualizaAux(galhoAux, nohAntigo);
			}return NULL;//caso não retorna NULL, que será "erro"
		}
	}
}

void arvoreBplus::atualizaPacoteArvore(pacote pacoteAntigo, pacote pacote1){
	noh* nohAntigo = new noh(pacoteAntigo, -1, -1);//pego o noh q quero procura
	galho* galhoAux = this->raiz;
	noh* nohAtualiza = this->atualizaAux(galhoAux, nohAntigo);//procuro ele
	if(nohAtualiza == NULL){//vejo se achei
		std::cout<<"Erro"<<std::endl;
	}else{
		nohAtualiza->atualizaPacoteNoh(pacote1);//atualizo ele
	}
}

void arvoreBplus::atualizaIndiceArvore(pacote pacoteAntigo, int indiceAnteriorPacketNovo){
	noh* nohAntigo = new noh(pacoteAntigo, -1, -1);//pego o noh q quero procura
	galho* galhoAux = this->raiz;
	noh* nohAtualiza = this->atualizaAux(galhoAux, nohAntigo);//procuro ele
	if(nohAtualiza == NULL){//vejo se achei
		std::cout<<"Erro"<<std::endl;
	}else{
		nohAtualiza->atualizaIndiceAnteriorNoh(indiceAnteriorPacketNovo);//atualizo ele
	}
}
