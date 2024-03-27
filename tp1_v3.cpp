#include <iostream>
#include <vector>
#include <stack>
#include <set>

// cria os vectors que armazenam as informações fundamentais
std::vector<int> _abertura;
std::vector<int> _low;
std::vector<int> _pai;
std::vector<int> _filhos;

// cria o set que armazenará quais vértices são links
std::set<int> links;
// contador do número de clusters que o grafo tem
int n_clusters = 0;

// essa classe armazena as arestas que serão inseridas na _pilha 
class Aresta_stack {
    private:
        int vertice1;
        int vertice2;
    public:
        Aresta_stack(int u, int v) {
            vertice1 = u;
            vertice2 = v;
        }
        int get_vert1() { return vertice1; }
        int get_vert2() { return vertice2; }
};

std::stack<Aresta_stack> _pilha; // armazena as arestas que formarão um cluster
std::set<std::set<int> > clusters; // armazena os clusters do grafo
std::set<std::set<int> > floresta; // armazena a floresta

// desempilha a pilha (a cada execução dessa função, recupera-se um cluster)
void get_clusters(int u, int v) {
    std::set<int> vertices_cluster;
    
    n_clusters++;
    while(_pilha.top().get_vert1() != u || _pilha.top().get_vert2() != v) {
        vertices_cluster.insert(_pilha.top().get_vert1());
        vertices_cluster.insert(_pilha.top().get_vert2());
        _pilha.pop();
    }
    vertices_cluster.insert(_pilha.top().get_vert1());
    vertices_cluster.insert(_pilha.top().get_vert2());
    _pilha.pop();

    clusters.insert(vertices_cluster); // insere o cluster no set 'clusters'
}


void dfs(std::vector<std::vector<int> > grafo, int index_vertice, int pai, int tempo, std::vector<int>& adjacentes) {

    // atribui os valores de abertura e de low do vértice que está sendo visitado pela primeira vez
    tempo += 1;
    _abertura[index_vertice] = tempo;
    _low[index_vertice] = tempo;
    // std::vector<int>& adjacentes = grafo[index_vertice];

    // percorre todos os vértices adjacentes do vértice corrente
    for(int i = 0; i < adjacentes.size(); i++) {
        // int proximo = adjacentes[i];

        // se o vértice adjacente ainda não foi visitado
        if(_abertura[adjacentes[i]-1] == 0) {
            _filhos[index_vertice] += 1;
            _pai[adjacentes[i]-1] = index_vertice+1; // define o pai do vértice adjacente
            // adiciona a aresta à pilha que guarda os clusters
            _pilha.push(Aresta_stack(index_vertice+1, adjacentes[i]));

            // chama a si mesma (recursão), para explorar os vértices vizinhos do filho que está sendo explorado no momento
            dfs(grafo, adjacentes[i]-1, index_vertice+1, tempo, grafo[adjacentes[i]-1]);

            if(_low[adjacentes[i]-1] < _low[index_vertice]) {
                _low[index_vertice] = _low[adjacentes[i]-1];
            }
            
            // encontra um link (vértice de corte)
            if(_pai[index_vertice] != -1 && _low[adjacentes[i]-1] >= _abertura[index_vertice]) {
                links.insert(index_vertice+1);               
            }

            // desempilha, resgatando um cluster
            if((_abertura[index_vertice] == 1 && _filhos[index_vertice] > 1) || (_abertura[index_vertice] > 1 && _low[adjacentes[i]-1] >= _abertura[index_vertice])) {
                get_clusters(index_vertice+1, adjacentes[i]);
            }

        } else if(adjacentes[i] != _pai[index_vertice]) {
            if(_abertura[adjacentes[i]-1] < _low[index_vertice]) {
                _low[index_vertice] = _abertura[adjacentes[i]-1];
            }
            if(_abertura[adjacentes[i]-1] < _abertura[index_vertice]) {
                _pilha.push(Aresta_stack(index_vertice, adjacentes[i]));
            }
        }
    }

    // encontra um link (vértice de corte)
    if(_pai[index_vertice] == -1 && (_filhos[index_vertice] > 1 || _filhos[index_vertice] == 0)) {
        links.insert(index_vertice+1);      
    }
}


int main() {
    int n_vertices, n_arestas;
    int tempo = 0;
    scanf("%d %d", &n_vertices, &n_arestas); // salva os valores das variáveis 'n_vertices' e 'n_arestas'

    std::vector<std::vector<int> > grafo;

    // adiciona os vértices no grafo
    for(int i = 0; i < n_vertices; i++) {
        std::vector<int> adjacentes;
        grafo.push_back(adjacentes);

        _abertura.push_back(0);
        _low.push_back(0);
        _pai.push_back(-1);
        _filhos.push_back(0);
    }

    int i;
    // salva os vértices adjacentes de cada vértice no vector "_adjacentes" de cada nó do grafo
    for(i = 0; i < n_arestas; i++) {
        int vertice1, vertice2;
        scanf("%d %d", &vertice1, &vertice2);

        grafo[vertice1-1].push_back(vertice2);
        grafo[vertice2-1].push_back(vertice1);
    }

    // chama a função dfs para cada vértice que ainda não foi visitado
    std::set<int> vertices_cluster;
    for(i = 0; i < n_vertices; i++) {
        if(_abertura[i] == 0) {
            // quando encontra um vértice que ainda não foi visitado, checa se _pilha tá vazia. Se não estiver, desempilha o que ainda está presente, criando um novo cluster
            if(_pilha.size() != 0) {
                while(!_pilha.empty()) {
                    vertices_cluster.insert(_pilha.top().get_vert1());
                    vertices_cluster.insert(_pilha.top().get_vert2());
                    _pilha.pop();
                }
                clusters.insert(vertices_cluster);
                n_clusters++;
            }

            dfs(grafo, i, -1, tempo, grafo[i]); // chama a função para o vértice cujo índice é 'i', que ainda não foi visitado (não tem pai)
        }
    }

    int tam_links = links.size(); // armazena o número de links que o vértice tem
    printf("%d\n", tam_links);
    for(std::set<int>::iterator itr = links.begin(); itr != links.end(); itr++) {
        printf("%d\n", *itr); // imprime os links do grafo
    }

    // se ainda tiver elementos de _pilha que não foram desempilhados, desempilha, criando um novo cluster
    if(_pilha.size() > 0) {
        std::set<int> vertices_cluster;
        while(!_pilha.empty()) {
            vertices_cluster.insert(_pilha.top().get_vert1());
            vertices_cluster.insert(_pilha.top().get_vert2());
            _pilha.pop();
        }
        clusters.insert(vertices_cluster);
        n_clusters++;
    }

    printf("%d\n", n_clusters); // imprime o número de clusters que compõe os grafo

    i = 1;
    int j = 1;
    int n_arestas_floresta = 0; // contabiliza o número de arestas da floresta

    std::set<std::set<int> >::iterator itr1;
    std::set<int>::iterator itr;
    // percorre o set 'clusters', para imprimir quais vértices compõe cada cluster
    for(itr1 = clusters.begin(); itr1 != clusters.end(); itr1++) {
        printf("%d %lu ", n_vertices+i, itr1->size());

        itr = itr1->begin(); 
        // percorre cada set que compõe o set 'clusters' e se o elemento percorrido for um link, adiciona o link e o número de representa o cluster que está sendo percorrido no set 'floresta'
        while (itr != itr1->end()) {
            if(links.find(*itr) != links.end()) {
                std::set<int> elem_floresta;
                elem_floresta.insert(n_vertices+i);
                elem_floresta.insert(*itr);
                floresta.insert(elem_floresta);
                n_arestas_floresta++;
            }
            if(j == itr1->size()) printf("%d\n", *itr);
            else printf("%d ", *itr);
            j++;
            itr++;
        }
        j = 1;
        i++;
    }

    // imprime cada aresta que compõe a floresta
    printf("%d %d\n", n_clusters+tam_links, n_arestas_floresta);
    for(itr1 = floresta.begin(); itr1 != floresta.end(); itr1++) {
        printf("%d %d\n", *(itr1->begin()), *(itr1->rbegin()));
    }
}