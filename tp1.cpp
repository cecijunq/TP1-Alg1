#include <iostream>
#include <vector>
#include <stack>
#include <set>

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

class Vertice {
    // armazena as informações de cada vértice do grafo
    private: 
        int _rotulo;
        std::vector<int> _adjacentes;
        int _abertura;
        int _low;
        int _filhos;
    
    public:
        Vertice(int rotulo) {
            _rotulo = rotulo;
            _abertura = 0;
            _filhos = 0;
        }
        int get_rotulo() { return _rotulo; }
        void set_abertura(int abertura) { _abertura = abertura; }
        int get_abertura() { return _abertura; }
        void set_low(int low) { _low = low; }
        int get_low() { return _low; }
        void add_adj(int adjacente) {_adjacentes.push_back(adjacente); }
        std::vector<int> get_adj() { return _adjacentes; }
        int get_filhos() { return _filhos; }
        void set_filhos() { _filhos++; }

};

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


void dfs(std::vector<Vertice *> grafo, int index_vertice, int pai, int tempo) {
    
    tempo++;
    grafo[index_vertice]->set_abertura(tempo);
    grafo[index_vertice]->set_low(tempo);
    std::vector<int> adjacentes = grafo[index_vertice]->get_adj();

    Vertice *corrente = grafo[index_vertice];
    for(int i = 0; i < adjacentes.size(); i++) {
        // Vertice *proximo = grafo[adjacentes[i]-1];

        if(grafo[adjacentes[i]-1]->get_abertura() == 0) {
            corrente->set_filhos();
            _pilha.push(Aresta_stack(corrente->get_rotulo(), grafo[adjacentes[i]-1]->get_rotulo()));

            dfs(grafo, adjacentes[i]-1, index_vertice+1, tempo);

            if(grafo[adjacentes[i]-1]->get_low() < corrente->get_low()) {
                int novo_low = grafo[adjacentes[i]-1]->get_low();
                corrente->set_low(novo_low);
            }
            
            if(pai != -1 && grafo[adjacentes[i]-1]->get_low() >= corrente->get_abertura()) {
                links.insert(index_vertice+1);               
            }

            if((corrente->get_abertura() == 1 && corrente->get_filhos() > 1) || (corrente->get_abertura() > 1 && grafo[adjacentes[i]-1]->get_low() >= corrente->get_abertura())) {
                get_clusters(corrente->get_rotulo(), grafo[adjacentes[i]-1]->get_rotulo());
            }

        } else if(grafo[adjacentes[i]-1]->get_rotulo() != pai) {
            if(grafo[adjacentes[i]-1]->get_abertura() < corrente->get_low()) {
                int novo_low = grafo[adjacentes[i]-1]->get_abertura();
                corrente->set_low(novo_low);
            }
            if(grafo[adjacentes[i]-1]->get_abertura() < corrente->get_abertura()) {
                _pilha.push(Aresta_stack(corrente->get_rotulo(), grafo[adjacentes[i]-1]->get_rotulo()));
            }
        }
    }

    if(pai == -1 && (corrente->get_filhos() > 1 || corrente->get_filhos() == 0)) {
        links.insert(index_vertice+1);      
    }
}


int main() {
    int n_vertices, n_arestas;
    int tempo = 0;
    scanf("%d %d", &n_vertices, &n_arestas); // salva os valores das variáveis 'n_vertices' e 'n_arestas'

    std::vector<Vertice *> grafo;

    // adiciona os vértices no grafo
    for(int i = 0; i < n_vertices; i++) {
        Vertice *node = new Vertice(i+1);
        grafo.push_back(node);
    }

    // salva os vértices adjacentes de cada vértice no vector "_adjacentes" de cada nó do grafo
    for(int i = 0; i < n_arestas; i++) {
        int vertice1, vertice2;
        scanf("%d %d", &vertice1, &vertice2);

        grafo[vertice1-1]->add_adj(vertice2);
        grafo[vertice2-1]->add_adj(vertice1);
    }

    // chama a função dfs para cada vértice que ainda não foi visitado
    std::set<int> vertices_cluster;
    for(int i = 0; i < n_vertices; i++) {
        if(grafo[i]->get_abertura() == 0) {
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

            dfs(grafo, i, -1, tempo); // chama a função para o vértice cujo índice é 'i', que ainda não foi visitado (não tem pai)
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

    int i = 1;
    int j = 1;
    int n_arestas_floresta = 0; // contabiliza o número de arestas da floresta

    // percorre o set 'clusters', para imprimir quais vértices compõe cada cluster
    for(std::set<std::set<int> >::iterator itr1 = clusters.begin(); itr1 != clusters.end(); itr1++) {
        printf("%d %lu ", n_vertices+i, itr1->size());

        std::set<int>::iterator itr = itr1->begin(); 
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
    for(std::set<std::set <int> >::iterator itr = floresta.begin(); itr != floresta.end(); itr++) {
        printf("%d %d\n", *(itr->begin()), *(itr->rbegin()));
    }

    for(int i = 0; i < n_vertices; i++) {
        delete grafo[i];
    }
}