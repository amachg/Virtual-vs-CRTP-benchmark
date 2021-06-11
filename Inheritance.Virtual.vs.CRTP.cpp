#include <iostream>
#include <chrono>

const unsigned N = 100'000;

//The Dynamically-polymorphic implementation
class DynamicInterface {
public:
  virtual void tick(uint64_t n) = 0;
  virtual uint64_t getvalue() = 0;
};

class DynamicImplementation : public DynamicInterface {
  uint64_t counter;
public:
  DynamicImplementation() : counter(0) {  }

  virtual void tick(uint64_t n) { counter += n; }

  virtual uint64_t getvalue() { return counter; }
};

void run_dynamic(DynamicInterface* obj) {
  for (unsigned i = 0; i < N; ++i) {
    for (unsigned j = 0; j < i; ++j) {
      obj->tick(j);
    }
  }
}

//The alternative statically-polymorphic implementation
template <typename Implementation>
class CRTPInterface {
public:
  void tick(uint64_t n) { impl().tick(n); }

  uint64_t getvalue() { return impl().getvalue(); }
private:
  Implementation& impl() { return *static_cast<Implementation*>(this); }
};

class CRTPImplementation : public CRTPInterface<CRTPImplementation> {
  uint64_t counter;
public:
  CRTPImplementation() : counter(0) { }

  void tick(uint64_t n) { counter += n; }

  uint64_t getvalue() { return counter; }
};

template <typename Implementation>
void run_crtp(CRTPInterface<Implementation>* obj) {
  for (unsigned i = 0; i < N; ++i) {
    for (unsigned j = 0; j < i; ++j) {
      obj->tick(j);
    }
  }
}

///////////////////////////////////////////////////////////////////////

template <typename ΜονάδαΧρόνου = std::chrono::milliseconds>
struct Χρονόμετρο {
	template<typename Functor, typename ...Args>
	static auto διάρκεια(Functor&& συναρτητής, Args&&... args) {
		// ξεκίνα χρονόμετρο
		const auto πριν = std::chrono::high_resolution_clock::now();
		std::invoke(std::forward<Functor>(συναρτητής), std::forward<Args>(args)...);
		// σταμάτα χρονόμετρο
		const auto τώρα = std::chrono::high_resolution_clock::now();
		return std::chrono::duration_cast<ΜονάδαΧρόνου>(τώρα - πριν);
	}
};

int main() {
    DynamicImplementation* dyna = new(DynamicImplementation);
	const auto χρονικήδιάρκεια1 = Χρονόμετρο<>::διάρκεια([&dyna]() { run_dynamic(dyna); });
    std::cout << "Dynamic (virtual): " << χρονικήδιάρκεια1.count() << " msecs\n";
	
    CRTPImplementation* sta = new(CRTPImplementation);
	const auto χρονικήδιάρκεια2 = Χρονόμετρο<>::διάρκεια([&sta]() { run_crtp(sta); });
    std::cout << "Static (C.R.T.P.): " << χρονικήδιάρκεια2.count() << " msecs\n";
}
