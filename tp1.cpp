#include <iostream>
#include <vector>
#include <stack>
#include <set>

std::set<int> links;
int n_clusters = 0;
int nv_floresta = 0;
int na_floresta = 0;

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

std::stack<Aresta_stack> _pilha;
std::set<std::set<int> > clusters;
std::set<std::set<int> > floresta;

class Vertice {
    private: 
        int _rotulo;
        bool _visited;
        int _pai;
        std::vector<int> _adjacentes;
        int _abertura;
        int _low;
        int _filhos;
        bool _eh_link;
    
    public:
        Vertice(int rotulo) {
            _rotulo = rotulo;
            _visited = false;
            _filhos = 0;
            _pai = 0;
            _eh_link = false;
        }
        int get_rotulo() { return _rotulo; }
        int get_visited() { return _visited; }
        void set_visited() { _visited = true; }
        void set_abertura(int abertura) { _abertura = abertura; }
        int get_abertura() { return _abertura; }
        void set_low(int low) { _low = low; }
        int get_low() { return _low; }
        void add_adj(int adjacente) {_adjacentes.push_back(adjacente); }
        std::vector<int> get_adj() { return _adjacentes; }
        int get_filhos() { return _filhos; }
        void set_filhos() { _filhos++; }
        void set_pai(int pai) { _pai = pai; }
        int get_pai() { return _pai; }
        void set_eh_link() { _eh_link = true; }
        bool get_eh_link() { return _eh_link; }

};

void get_clusters(std::vector<Vertice *> grafo, int u, int v) {
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
    clusters.insert(vertices_cluster);
}


void dfs(std::vector<Vertice *> grafo, int index_vertice, int pai, int tempo) {
    
    tempo++;
    grafo[index_vertice]->set_visited();
    grafo[index_vertice]->set_abertura(tempo);
    grafo[index_vertice]->set_low(tempo);
    std::vector<int> adjacentes = grafo[index_vertice]->get_adj();

    Vertice *corrente = grafo[index_vertice];
    for(int i = 0; i < adjacentes.size(); i++) {
        Vertice *proximo = grafo[adjacentes[i]-1];

        if(!proximo->get_visited()) {
            grafo[index_vertice]->set_pai(pai);
            corrente->set_filhos();
            _pilha.push(Aresta_stack(corrente->get_rotulo(), proximo->get_rotulo()));

            dfs(grafo, adjacentes[i]-1, index_vertice+1, tempo);

            if(proximo->get_low() < corrente->get_low()) {
                int novo_low = proximo->get_low();
                corrente->set_low(novo_low);
            }
            
            if(pai != -1 && proximo->get_low() >= corrente->get_abertura()) {
                links.insert(index_vertice+1);
                grafo[index_vertice]->set_eh_link();                
            }

            if((corrente->get_abertura() == 1 && corrente->get_filhos() > 1) || (corrente->get_abertura() > 1 && proximo->get_low() >= corrente->get_abertura())) {
                get_clusters(grafo, corrente->get_rotulo(), proximo->get_rotulo());
            }

        } else if(proximo->get_rotulo() != pai) {
            if(proximo->get_abertura() < corrente->get_low()) {
                int novo_low = proximo->get_abertura();
                corrente->set_low(novo_low);
            }
            if(proximo->get_abertura() < corrente->get_abertura()) {
                _pilha.push(Aresta_stack(corrente->get_rotulo(), proximo->get_rotulo()));
            }
        }
    }

    if(pai == -1 && (corrente->get_filhos() > 1 || corrente->get_filhos() == 0)) {
        links.insert(index_vertice+1);
        grafo[index_vertice]->set_eh_link();        
    }
}


int main() {
    int n_vertices, n_arestas;
    int tempo = 0;
    scanf("%d %d", &n_vertices, &n_arestas);

    std::vector<Vertice *> grafo;

    for(int i = 0; i < n_vertices; i++) {
        Vertice *node = new Vertice(i+1);
        grafo.push_back(node);
    }

    for(int i = 0; i < n_arestas; i++) {
        int vertice1, vertice2;
        scanf("%d %d", &vertice1, &vertice2);

        grafo[vertice1-1]->add_adj(vertice2);
        grafo[vertice2-1]->add_adj(vertice1);
    }

    std::set<int> vertices_cluster;
    for(int i = 0; i < n_vertices; i++) {
        if(grafo[i]->get_visited() == false) {
            if(_pilha.size() != 0) {
                while(!_pilha.empty()) {
                    vertices_cluster.insert(_pilha.top().get_vert1());
                    vertices_cluster.insert(_pilha.top().get_vert2());
                    _pilha.pop();
                }
                clusters.insert(vertices_cluster);
                n_clusters++;
            }

            dfs(grafo, i, -1, tempo);
        }
    }

    int tam_links = links.size();
    printf("%d\n", tam_links);
    for(std::set<int>::iterator itr = links.begin(); itr != links.end(); itr++) {
        printf("%d\n", *itr);
    }

    // std::set<int> vertices_cluster;
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

    printf("%d\n", n_clusters);

    int i = 1;
    int j = 1;
    int n_arestas_floresta = 0;

    for(std::set<std::set<int> >::iterator itr1 = clusters.begin(); itr1 != clusters.end(); itr1++) {
        printf("%d %lu ", n_vertices+i, itr1->size());

        std::set<int>::iterator itr = itr1->begin();
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

    printf("%d %d\n", n_clusters+tam_links, n_arestas_floresta);
    for(std::set<std::set <int> >::iterator itr = floresta.begin(); itr != floresta.end(); itr++) {
        printf("%d %d\n", *(itr->begin()), *(itr->rbegin()));
    }
}