/*
Trabalho de Estrutura de Dados - GCC216
Ordenacao em Memoria Secundaria, com Sequence Set
Copyright 2020 by Chrystian Arriel Amaral, Debora Rossini Martins Cardoso e Thiago Salles Santos
Arquivo leCSV.h: definição da class leCSV que faz leitura do arquivo .csv e insere no sequence set
*/

class leCSV{
    public:
    leCSV(SequenceSet& sequence, unsigned posInicio, unsigned quantLinhas, bool caso){
        std::ifstream arquivo_csv("captura_pacotes.csv");

        if (not(arquivo_csv)) {
            throw runtime_error("Erro ao abrir o arquivo .csv (leCSV)");
        }

        string linha;
        string campo; // variável para obter um campo de cada linha lida
        string delimitador = "\",\""; // delimitador entre os campos
        pacote umPacote;
        unsigned posFimCampo; // posição final do campo

        for(unsigned c=0; c<posInicio; c++) getline(arquivo_csv,linha); // pula linhas ate o inicio do trecho
        
        clock_t inicio = clock();
        cout << "Lendo arquivo...";
        for(unsigned c=0; c<quantLinhas+1; c++){
            getline(arquivo_csv,linha);

            campo = linha.erase(0,1); // remove primeiro caracter da linha (")

            // obtendo primeiro campo, um inteiro - No.
            posFimCampo = linha.find(delimitador);
            campo = linha.substr(0, posFimCampo);
            linha.erase(0, posFimCampo + delimitador.length());
            umPacote.indice = stoul(campo);

            // obtendo segundo campo, um float - Time
            posFimCampo = linha.find(delimitador);
            campo = linha.substr(0, posFimCampo);
            linha.erase(0, posFimCampo + delimitador.length());
            umPacote.tempo = stof(campo);

            // obtendo terceiro campo, um texto - Source
            posFimCampo = linha.find(delimitador);
            campo = linha.substr(0, posFimCampo);
            linha.erase(0, posFimCampo + delimitador.length());
            strcpy(umPacote.origem, campo.c_str());

            // obtendo quarto campo, um texto - Destination
            posFimCampo = linha.find(delimitador);
            campo = linha.substr(0, posFimCampo);
            linha.erase(0, posFimCampo + delimitador.length());
            strcpy(umPacote.destino, campo.c_str());

            // obtendo quinto campo, um texto - Protocol
            posFimCampo = linha.find(delimitador);
            campo = linha.substr(0, posFimCampo);
            linha.erase(0, posFimCampo + delimitador.length());
            strcpy(umPacote.protocolo, campo.c_str());

            // obtendo sexto campo, um texto - Length
            posFimCampo = linha.find(delimitador);
            campo = linha.substr(0, posFimCampo);
            linha.erase(0, posFimCampo + delimitador.length());
            umPacote.tamanho = stoul(campo);

            // obtendo último campo, um texto - Info
            // nesse caso, procuro o último aspas, fazendo busca reversa
            posFimCampo = linha.rfind("\"");
            campo = linha.substr(0, posFimCampo);
            strcpy(umPacote.infomarcao, campo.c_str());

            sequence.insereElemento(umPacote, caso);  // insere no sequence set
        }
        arquivo_csv.close();
        clock_t fim = clock();
        
        cout << endl << "Arquivo lido com sucesso (tempo: " << (fim - inicio)/(double) CLOCKS_PER_SEC << " segundos)" << endl << endl;
    }
};
