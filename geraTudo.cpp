/*
 *  Script para povoar as tabelas, para o segundo trabalho de banco de dados
 *  Grupo: Francisco Thiesen, Gabriel Andrade, Matheus Kerber e Thiago Ribeiral.
 *  Horas de trabalho: 6
 * 
 */
#include <bits/stdc++.h>

using namespace std;

#define fi first
#define se second
#define all(v) (v).begin(), (v).end()
#define pb push_back
#define mp make_pair
#define sz(v) (int) (v).size()
#define rep(i,a,b) for(int (i) = (a); (i) < (b); ++(i))

typedef pair<int, int> ii;
typedef vector<int> vi;
typedef long long ll;

// Funcao que gera um data de nascimento
string generateBirthDate(int a, int b, int c) 
{
    string y = to_string(1970 + a % 30);
    string m = to_string(1 + b % 12);
    string d = to_string(1 + c % 30);
    if(sz(m) == 1){ m += '0'; reverse(all(m));}
    if(sz(d) == 1){ d += '0'; reverse(all(d));}
    string date = "'"; date += y; date += '/'; date += m; date += '/'; date += d; date += "'";
    return date;
}

// Funcao que gera uma string nome se separa = false, ou nome + sobrenome se separa = true
string generateName(bool separa, vi& x) {
    int nameLength = 5 + x[3] % 10;
    string name = "\'";
    rep(i, 0, 2 * nameLength) {
        if(i == nameLength && separa) name += ' ';
        char c = 'a';
        int inc = x[i] % 26;
        c = (c + inc);
        name += c;
    }
    name += '\'';
    return name;
}

string generateDate(int dia, int mes, int ano) {
    string y = to_string(ano);
    string m = to_string(mes);
    string d = to_string(dia);
    if(sz(m) == 1){ m += '0'; reverse(all(m));}
    if(sz(d) == 1){ d += '0'; reverse(all(d));}
    string date = "'"; date += y; date += '/'; date += m; date += '/'; date += d; date += "'";
    return date;
}

int main() {
    // vou comentar todas as minhas decisoes que vao ser hardcoded no script
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, 0x3f3f3f3f);
    uniform_real_distribution<> dis2(5, 10);
    // vou criar 10 torneios, dos quais apenas 5 [1, 5] vao ser de ponta 
    ofstream torneios          ("fillTournaments.txt");    
    ofstream de_ponta          ("fillDePonta.txt");
    ofstream competidor        ("fillCompetidor.txt");
    ofstream grupo             ("fillGrupo.txt"); 
    ofstream prova             ("fillProva.txt"); 
    ofstream inscricao         ("fillInscricao.txt");
    ofstream grupo_torneio     ("fillGrupoTorneio.txt");
    ofstream individual_torneio("fillIndividualTorneio.txt"); 
    //ofstream membro           ("fillMembro.txt");
    // Preenchendo os torneios passados
    rep(torneioId, 1, 11) {
        vi nums(40);
        rep(i,0,40) nums[i] = dis(gen);
        string name = generateName(false, nums);
        torneios << "INSERT INTO torneio (id, nome) VALUES (" << torneioId << "," << name << ");" << endl;
    }

    // Prechendo os torneios de grupo e individuais de ponta
    rep(idTorneio, 1, 6) {
        rep(idProva, 1, 25) {    
            de_ponta<< "INSERT INTO de_ponta (torneio, prova) VALUES (" << idTorneio << "," << idProva << ");" << endl;
        }
    }

    /*
     * Preenchendo competidor...
     * Considerar que os primeiros 500 sao homens e os outros 500 sao mulheres
     */

    int idCompetidorCorrente = 0;
    //vector<tuple<int, string, string> > comp;
    rep(i,0,1000) {
        int a = dis(gen), b = dis(gen), c = dis(gen);
        vi nums(40);
        rep(k, 0, 40) nums[k] = dis(gen);
        string name = generateName(true, nums);
        competidor << "INSERT INTO competidor (id, data_nascimento, nome, sexo) VALUES(" << idCompetidorCorrente++ << "," << "TO_DATE(" << generateBirthDate(a,b,c) << ", 'yyyy/mm/dd')" << "," << name << "," << (i / 500 == 0 ? "\'m\'" : "\'f\'" ) << ");" << endl;    
    }
    
    /*
     * Vou aproveitar que gerei 500 de cada, e fazer 125 grupos de homens e 125 grupos de mulheres.
     * Cada grupo vai ser composto de exatamente 4 pessoas, como é de praxe no revezamento de atletismo
     * Sobre id's de grupos. Os grupos de (1, 125) sao de homens e de (126, 250) sao de mulheres
     */
    vi randomiza(500, 0);
    iota(all(randomiza), 0);
    random_shuffle(all(randomiza));
    int idGrupo = 0; 
  
    // Lembrando que convencionei que os grupos vao ser sempre de 4 pessoas
    vector<vector<int> > gruposMasc(125);
    vector<vector<int> > gruposFem(125);

    // Preenchendo grupos de homens
    rep(i,0,500) {
        //membro << "INSERT INTO membro (competidor, grupo) VALUES (" << randomiza[i] << "," << i / 4 << ")" << endl;
        gruposMasc[i/4].push_back(randomiza[i]); 
    }

    iota(all(randomiza), 500);
    random_shuffle(all(randomiza));
    // Preenchendo grupos de mulheres 
    rep(i, 500, 1000) {
        //membro << "INSERT INTO membro (competidor, grupo) VALUES (" << randomiza[i] << "," << i / 4 << ")" << endl;
        gruposFem[(i-500) / 4].push_back(randomiza[i - 500]);
    }

    rep(i, 0, 250) {
        vi nums(40);
        rep(k,0,40) nums[k] = dis(gen);
        grupo << "INSERT INTO grupo (id, nome) VALUES (" << i << "," << generateName(false, nums) << ");" << endl;
    }
    
    // Agora vou fazer a logica de preenchimento das provas
    // Sao um total de 24 provas
    // Vou convencionar aqui que as provas \in [1,12] sao masculinas
    // E as provas [13,24] sao femininas
    // Vou tirar o id da prova % 3 para estipular qual a distancia da prova.
    /*
     * id % 3 == 1 -> 'c'
     * id % 3 == 2 -> 'm'
     * id % 3 == 0 -> 'l'
     *
     *
     * As Provas de [1,6] e [13,18] vao ser provas de grupo
     * E as provas [7,12] e [19,24] vao ser individuais
     *
     * Para determinar se a prova vai ou nao ser com barreiras, vou olhar o id da prova % 2
     * id % 2 == 0 -> Sem barreiras
     * id % 2 == 1 -> Com barreiras
     */
    
    // Estou tambem convencionando que todas as inscriçoes foram realizadas ate o 04/02/2018
    
    // Vou aproveitar e ja selecionar aqui para cada prova os id's dos que vao se inscrever nela
    vector<vector<int> > individuosQueSeInscrevemNaProva(25);
    vector<vector<int> > gruposQueSeInscrevemNaProva(25); // so vai estar preenchido se for prova de grupo

    constexpr int gruposInscritosPorProva = 20;
    constexpr int individuosInscritosPorProva = 70;
    
    unordered_map<int, int> provasRealizadas;


    int diaAtual = 0;
    // Vamos para simplificar as coisas supor que estamos começando do mes de março.. E que todos os meses tem 30 dias e por motivos religiosos nenhuma bateria vai ocorrer no dia 31 de algum mes
    int anoAtual = 2018; 
    rep(idProva, 1, 25) {
        string genero      = (idProva <= 12) ? "\'m\'" : "\'f\'";
        string comBarreira = (idProva % 2)   ? "1" : "0";
        string distancia;
        string deGrupo     = "0";
        if(idProva >= 1 && idProva <= 6) deGrupo = "1";
        if(idProva >= 13 && idProva <= 18) deGrupo = "1";

        if(idProva%3 == 1) distancia = "100";
        else if(idProva%3 == 2) distancia = "200";
        else distancia = "400";
        
        // Provas de grupo nao possuem oitavas ou quartas         
        
        // Numa prova de grupo temos que ter ao menos  16 times inscritos
        // Numa prova individual temos que ter ao menos 64 inscritos
        
        /*
         * Para testar se a selecao de inscritos esta funcionando corretamente,
         * Vou selecionar 20 grupos para a prova de grupos e
         * 70 participantes para cada prova individual.
         * Para fins de testes esse parametro pode ser alterado depois.
         */

        if(deGrupo == "1") {
            prova << "INSERT into prova (id, sexo, distancia, barreiras, grupo, data_final, data_semi) VALUES(";
            prova << idProva << ", " << genero << ", " << distancia << ", " << comBarreira << ", " << deGrupo << ", ";
            while(provasRealizadas[diaAtual] >= 3 ) diaAtual++;
            provasRealizadas[diaAtual]++;
            provasRealizadas[diaAtual + 1]++;

            prova  << "TO_DATE(" << generateDate( (diaAtual + 1) % 30 + 1, (diaAtual + 1) / 30 + 3, anoAtual) << ", 'yyyy/mm/dd')" << ", " << "TO_DATE(" << generateDate( diaAtual % 30 + 1, diaAtual / 30 + 3, anoAtual) << ", 'yyyy/mm/dd')" << ");" << endl;
            set<int> G; // vou usar set para num primeiro momento impedir que o mesmo time se cadastre 2x na mesma prova
            while(sz(G) < gruposInscritosPorProva ) {
                int nxt = dis(gen) % 125;
                G.insert(nxt);
            }
            if( genero == "\'m\'" ) {
                for(const auto& v : G) {
                    gruposQueSeInscrevemNaProva[idProva].push_back(v);
                    for(const auto& membro : gruposMasc[v] ) {
                        individuosQueSeInscrevemNaProva[idProva].push_back(membro);
                    }
                }
            }
            else {
                for(const auto& v : G) {
                    gruposQueSeInscrevemNaProva[idProva].push_back(v + 125);
                    for(const auto& membro : gruposFem[v] ) {
                        individuosQueSeInscrevemNaProva[idProva].push_back(membro);
                    }
                }
            }
        }
        else {
            prova << "INSERT into prova (id, sexo, distancia, barreiras, grupo, data_final, data_semi, data_quartas, data_oitavas) VALUES (";
            prova << idProva << ", " << genero << ", " << distancia << ", " << comBarreira << ", " << deGrupo << ", ";
            while( provasRealizadas[diaAtual] >= 3) ++diaAtual;
            rep(i, 0, 4) provasRealizadas[diaAtual]++;
            prova << "TO_DATE(" << generateDate( (diaAtual + 3) % 30 + 1, (diaAtual + 3) / 30 + 3, anoAtual) << ", 'yyyy/mm/dd')" << ", " << "TO_DATE(" << generateDate( (diaAtual + 2) % 30 + 1, (diaAtual + 2) / 30 + 3 , anoAtual) << ", 'yyyy/mm/dd')" << ", ";
            prova << "TO_DATE(" << generateDate( (diaAtual + 1) % 30 + 1, (diaAtual + 1) / 30 + 3, anoAtual) << ", 'yyyy/mm/dd')" << ", " << "TO_DATE(" << generateDate(diaAtual % 30 + 1, diaAtual / 30 + 3, anoAtual) << ", 'yyyy/mm/dd'));" << endl;
            set<int> G;
            while(sz(G) < individuosInscritosPorProva ) {
                int nxt = dis(gen) % 500;
                G.insert(nxt);
            }
            if( genero == "\'m\'" ) {
                for(const auto& ind : G) {
                    individuosQueSeInscrevemNaProva[idProva].push_back(ind);
                }
            }
            else {
                for(const auto& ind : G) {
                    individuosQueSeInscrevemNaProva[idProva].push_back(ind + 500);
                }
            }
        }
    }
    
    // Hora de preencher as inscricoes
    rep(idProva, 1, 25) {
        bool deGrupo = ( idProva >= 1 && idProva <= 6) || ( idProva >= 13 && idProva <= 18);
        if(deGrupo) {
            for(const int g : gruposQueSeInscrevemNaProva[idProva] ) {
                if(g >= 125) {
                    for(const int membro : gruposFem[g - 125] ) {
                        inscricao << "INSERT into inscricao (competidor, prova, grupo) VALUES (" << membro << "," << idProva << "," << g << ");" << endl;
                    }
                }
                else {
                    for(const int membro : gruposMasc[g] ) {
                        inscricao << "INSERT into inscricao (competidor, prova, grupo) VALUES (" << membro << "," << idProva << "," << g << ");" << endl;
                    }
                } 
            }
        }
        else {
            for(const int ind : individuosQueSeInscrevemNaProva[idProva] ) {
                inscricao << "INSERT into inscricao (competidor, prova) VALUES (" << ind << "," << idProva << ");" << endl;
            }
        }
    } 
    // Agora vou preencher para todos os individuos o melhor tempo deles em uma prova de ponta
    // Tambem vou preencher para todos os grupos
    // Vale lembrar que os torneios de ponta de id's de 1 a 5
    rep(idCompetidor, 0, 1000) {
        int t = 1 + ( dis(gen) % 5); 
        double tempo = dis2(gen);
        individual_torneio << "INSERT into individual_torneio (competidor, torneio, tempo) VALUES (" << idCompetidor << "," << t << "," << tempo << ");" << endl;
    }
    rep(idGrupo, 0, 250) {
        int t = 1 + ( dis(gen) % 5);
        double tempo = dis2(gen);
        grupo_torneio << "INSERT into grupo_torneio (competidor, torneio, tempo) VALUES (" << idGrupo << "," << t << "," << tempo << ");" << endl;
    }
    // Vou deixar comentado esse trecho de codigo que gera tempos para os competidores em torneios que n sao de ponta
    /*rep(idCompetidor, 0, 1000) {
        int t = 6+ ( dis(gen) % 5); 
        double tempo = dis2(gen);
        individual_torneio << "INSERT into individual_torneio (competidor, torneio, tempo) VALUES (" << idCompetidor << "," << t << "," << tempo << ");" << endl;
    }
    rep(idGrupo, 0, 250) {
        int t = 6 + ( dis(gen) % 5);
        double tempo = dis2(gen);
        grupo_torneio << "INSERT into grupo_torneio (competidor, torneio, tempo) VALUES (" << idGrupo << "," << t << "," << tempo << ");" << endl;
    }*/

    return 0;
}
