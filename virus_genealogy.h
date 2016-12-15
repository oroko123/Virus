#ifndef VIRUS_GENEALOGY_H
#define VIRUS_GENEALOGY_H

#include <map>
#include <set>
#include <vector>
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
	// !!! wyjątki w konstruktorze?
private:
    typedef typename Virus::id_type id_type;

    class VirusHolder {
	// !!! wyjątki w konstruktorze?
  // tu mi się wydaje, że skoro pierwszą instrukcją jest ten konstruktor, to powinno być ok
    public:
        std::unique_ptr <Virus> virus;
        std::set<id_type> parents;
        std::set<id_type> children;

        VirusHolder(id_type const &_id, std::vector<id_type> const &_parents) {
            this->virus = std::unique_ptr <Virus> (new Virus(_id));
            this->parents = std::set <id_type> (_parents.begin(), _parents.end());
        }

        VirusHolder(id_type const &_id) {
            VirusHolder(_id, std::vector<id_type>());
        }

        VirusHolder() { }
    };

    std::map<id_type, VirusHolder> genealogy;
    id_type stem_id;


    /// strong
    /// [why]: operator [i] jest strong
    void create_stem(id_type const &id) {
        genealogy[id] = VirusHolder(id);
    }


public:
    VirusGenealogy(id_type const &_stem_id) {
        create_stem(_stem_id);
        stem_id = _stem_id;
    }
    VirusGenealogy(const VirusGenealogy& gen) = delete;


	/// no-throw
	/// [why]: zwraca prywatne pole
    id_type get_stem_id() const noexcept {
        return stem_id;
    }

	/// strong
	/// [why]: metoda count jest strong
    bool exists(id_type const &id) const {
			return genealogy.find(id) != genealogy.end();
    }


	/// strong
	/// [why]: metoda find jest strong
    Virus &operator[](id_type const &id) const {
        if (!exists(id)) {
            throw VirusNotFound();
        }
        return *genealogy.find(id)->second.virus;
    }

	/// strong
	/// [why]: metoda find jest strong
    std::vector<id_type> get_children(id_type const &id) const {
        if (!exists(id)) {
            throw VirusNotFound();
        }
        return std::vector <id_type>
            (genealogy.find(id)->second.children.begin(),
            genealogy.find(id)->second.children.end());
    }

	/// strong
	/// [why]: metoda find jest strong
    std::vector<id_type> get_parents(id_type const &id) const {
        if (!exists(id)) {
            throw VirusNotFound();
        }
        return std::vector <id_type>
            (genealogy.find(id)->second.parents.begin(),
            genealogy.find(id)->second.parents.end());
    }

	/// strong
	/// [why]: metoda find jest strong
    void create(id_type const &id, id_type const &parent_id) {
        std::vector <id_type> parent_ids;
        parent_ids.push_back(parent_id);
        create(id, parent_ids);
    }

	/// strong
	/// [why]: implementacja + insert jest strong
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

        /// oddzielenie od wyjątkogennego konstruktora
        /// w ten sposob gwarantujemy to, ze obiekt pod adresem genealogy[id]
        /// się nie zmieni
        // !!! : powinno być tak : genealogy[id] = vh;

        std::vector <typename std::set<id_type>::iterator> v;
        try {
          for (id_type parent : parent_ids) {
            v.push_back(genealogy[parent].children.insert(id).first);
          }
          VirusHolder vh = VirusHolder(id, parent_ids);
          genealogy[id] = VirusHolder(id, parent_ids);
        } catch (const std::exception &e) {
          for (size_t i = 0; i < v.size(); i++) {
            genealogy[parent_ids[i]].children.erase(v[i]);
          }
          throw e;
        }
    }

	/// strong
	/// [why]: exists jest strong
    void connect(id_type const &child_id, id_type const &parent_id) {
        if (!exists(child_id) || !exists(parent_id)) {
            throw VirusNotFound();
        }

        /*
        std::set<id_type> p = genealogy[child_id].parents;
        /// If a single element is to be inserted, there are no changes
		/// in the container in case of exception (strong guarantee).
		p.insert(parent_id);
        genealogy[child_id].parents = p;
        std::set<id_type> ch = genealogy[parent_id].children;
        /// If a single element is to be inserted, there are no changes
		/// in the container in case of exception (strong guarantee).
		ch.insert(child_id);
        genealogy[parent_id].children = ch;
         */

        //Wydaje mi się, że tu problemem nie jest insert(i tak wrzucamy 1 element)
        //tylko właśnie to, że dorzucamy w 2 miejscach i wyjątek może się pojawić przy późniejszym dorzuceniu
        //kopiowanie całego seta jest bardzo nieefektywne
        //ja zrobiłbym tak:

        /// If a single element is to be inserted, there are no changes
        /// in the container in case of exception (strong guarantee).
        auto it =   genealogy[child_id].parents.insert(parent_id).first;
        try {
            genealogy[parent_id].children.insert(child_id);
        } catch (const std::exception &e) {
            ///erase with iterator parameter is no-throw
            genealogy[child_id].parents.erase(it);
            throw e;
        }
    }

	/// strong
	/// [why]: erase, remove jest strong, empty jest no-throw
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
			std::set<id_type> p =  genealogy[child_id].parents;
			p.erase(id);
			genealogy[child_id].parents = p;

            if (genealogy[child_id].parents.empty()) {
                remove(child_id);
            }
        }
        // !!! z tym coś trzeba zrobić
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
