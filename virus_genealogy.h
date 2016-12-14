#ifndef VIRUS_GENEALOGY_H
#define VIRUS_GENEALOGY_H

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <exception>
#include <iostream>
#include <memory>

class VirusNotFound : public std::exception {
    virtual const char *what() const throw() {
        return "VirusNotFound";
    }
};

class VirusAlreadyCreated : public std::exception {
    virtual const char *what() const throw() {
        return "VirusAlreadyCreated";
    }
};

class TriedToRemoveStemVirus : public std::exception {
    virtual const char *what() const throw() {
        return "TriedToRemoveStemVirus";
    }
};

template<class Virus>
class VirusGenealogy {
private:
    typedef typename Virus::id_type id_type;

    class VirusHolder {
    public:
        std::unique_ptr <Virus> virus;
        std::unordered_set<id_type> parents;
        std::unordered_set<id_type> children;

        VirusHolder(id_type const &_id, std::vector<id_type> const &_parents) {
            this->virus = std::unique_ptr <Virus> (new Virus(_id));
            this->parents = std::unordered_set <id_type> (_parents.cbegin(), _parents.cend());
        }

        VirusHolder(id_type const &_id) {
            VirusHolder(_id, std::vector<id_type>());
        }

        VirusHolder() { }
    };

    std::unique_ptr <Virus> stem;
    std::unordered_map<id_type, VirusHolder> genealogy;

public:
    VirusGenealogy(id_type const &stem_id) {
        stem = std::unique_ptr <Virus> (new Virus(stem_id));
        create_stem(stem_id);
    }
    VirusGenealogy(const VirusGenealogy& gen) = delete;

    id_type get_stem_id() const {
        return stem->get_id();
    }

    bool exists(id_type const &id) const {
        return genealogy.count(id) == 1;
    }

    Virus &operator[](id_type const &id) const {
        if (!exists(id)) {
            throw VirusNotFound();
        }
        return *genealogy.find(id)->second.virus;
    }

    std::vector<id_type> get_children(id_type const &id) const {
        if (!exists(id)) {
            throw VirusNotFound();
        }
        return std::vector <id_type>
            (genealogy.find(id)->second.children.begin(),
            genealogy.find(id)->second.children.end());
    }

    std::vector<id_type> get_parents(id_type const &id) const {
        if (!exists(id)) {
            throw VirusNotFound();
        }
        return std::vector <id_type>
            (genealogy.find(id)->second.parents.begin(),
            genealogy.find(id)->second.parents.end());
    }

	 void create_stem(id_type const &id) noexcept {
        genealogy[id] = VirusHolder(id, std::vector<id_type>());
    }

    void create(id_type const &id, id_type const &parent_id) {
        std::vector <id_type> parent_ids;
        parent_ids.push_back(parent_id);
        create(id, parent_ids);
    }

    void create(id_type const &id, std::vector<id_type> const &parent_ids) {
        if (exists(id)) {
            throw VirusAlreadyCreated();
        }
        if (parent_ids.size() == 0) {
			throw VirusNotFound();
		}
        for (id_type parent_id : parent_ids) {
            if (!exists(parent_id)) {
                throw VirusNotFound();
            }
        }
        genealogy[id] = VirusHolder(id, parent_ids);
        for (id_type parent : parent_ids) {
            genealogy[parent].children.insert(id);
        }
    }

    void connect(id_type const &child_id, id_type const &parent_id) {
        if (genealogy.find(child_id) == genealogy.end() ||
            genealogy.find(parent_id) == genealogy.end()) {
            throw VirusNotFound();
        }
        genealogy[child_id].parents.insert(parent_id);
        genealogy[parent_id].children.insert(child_id);
    }

    void remove(id_type const &id) {
        if (!exists(id)) {
            throw VirusNotFound();
        }
        if (get_stem_id() == id) {
            throw TriedToRemoveStemVirus();
        }
        for (id_type parent_id : genealogy[id].parents) {
            genealogy[parent_id].children.erase(id);
        }
        for (id_type child_id : genealogy[id].children) {

            genealogy[child_id].parents.erase(id);

            if (genealogy[child_id].parents.empty()) {
                remove(child_id);
            }
        }
        genealogy.erase(id);
    }
};


/*
// Tworzy nową genealogię.
// Tworzy także węzeł wirusa macierzystego o identyfikatorze stem_id.
Virus::VirusGenealogy(Virus::id_type const &stem_id);

// Zwraca identyfikator wirusa macierzystego.
Virus::id_type get_stem_id() const;

// Zwraca listę identyfikatorów bezpośrednich następników wirusa
// o podanym identyfikatorze.
// Zgłasza wyjątek VirusNotFound, jeśli dany wirus nie istnieje.
std::vector<Virus::id_type> get_children(Virus::id_type const &id) const;

// Zwraca listę identyfikatorów bezpośrednich poprzedników wirusa
// o podanym identyfikatorze.
// Zgłasza wyjątek VirusNotFound, jeśli dany wirus nie istnieje.
std::vector<Virus::id_type> get_parents(Virus::id_type const &id) const;

// Sprawdza, czy wirus o podanym identyfikatorze istnieje.
bool exists(Virus::id_type const &id) const;

// Zwraca referencję do obiektu reprezentującego wirus o podanym
// identyfikatorze.
// Zgłasza wyjątek VirusNotFound, jeśli żądany wirus nie istnieje.
Virus& operator[](Virus::id_type const &id) const;

// Tworzy węzeł reprezentujący nowy wirus o identyfikatorze id
// powstały z wirusów o podanym identyfikatorze parent_id lub
// podanych identyfikatorach parent_ids.
// Zgłasza wyjątek VirusAlreadyCreated, jeśli wirus o identyfikatorze
// id już istnieje.
// Zgłasza wyjątek VirusNotFound, jeśli któryś z wyspecyfikowanych
// poprzedników nie istnieje.
void create(Virus::id_type const &id, Virus::id_type const &parent_id);
void create(Virus::id_type const &id, std::vector<Virus::id_type> const &parent_ids);


// Dodaje nową krawędź w grafie genealogii.
// Zgłasza wyjątek VirusNotFound, jeśli któryś z podanych wirusów nie istnieje.
void connect(Virus::id_type const &child_id, virus::id_type const &parent_id);

 TODO
// Usuwa wirus o podanym identyfikatorze.
// Zgłasza wyjątek VirusNotFound, jeśli żądany wirus nie istnieje.
// Zgłasza wyjątek TriedToRemoveStemVirus przy próbie usunięcia
// wirusa macierzystego.
void remove(Virus::id_type const &id);
*/

#endif //VIRUS_GENEALOGY_H
