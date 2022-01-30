//Inheritance: Virtual vs. CRTP

#include <iostream>
#include <chrono>

const unsigned N = 50'000;

//The Dynamically-polymorphic
class DynamicInterface {
public:
  virtual void tick(uint64_t n) = 0;
  virtual uint64_t getvalue() = 0;
};

class DynamicImplementation : public DynamicInterface {
public:
  DynamicImplementation() : counter(0) {  }

  virtual void tick(uint64_t n) { counter += n; }

  virtual uint64_t getvalue() { return counter; }
private:
  uint64_t counter;
};

void work(DynamicInterface&& obj) {
  for (unsigned i = 0; i < N; ++i) {
    for (unsigned j = 0; j < i; ++j) {
      obj.tick(j);
    }
  }
}

//The alternative statically-polymorphic
template <typename Implementation>
class CRTPInterface {
public:
  void tick(uint64_t n) { impl().tick(n); }

  uint64_t getvalue() { return impl().getvalue(); }
private:
  Implementation& impl() { return *static_cast<Implementation*>(this); }
};

class CRTPImplementation : public CRTPInterface<CRTPImplementation> {
public:
  CRTPImplementation() : counter(0) { }

  void tick(uint64_t n) { counter += n; }

  uint64_t getvalue() { return counter; }
private:
  uint64_t counter;
};

template <typename Implementation>
void work(CRTPInterface<Implementation>&& obj) {
  for (unsigned i = 0; i < N; ++i) {
    for (unsigned j = 0; j < i; ++j) {
      obj.tick(j);
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
    std::cout << "Main started \n";

	const auto dyn_διάρκεια = Χρονόμετρο<>::διάρκεια([]() {
		work(DynamicImplementation{});
		});
    std::cout << "Dynamic (virtual): " << dyn_διάρκεια.count() << " msecs\n";
	
	const auto sta_διάρκεια = Χρονόμετρο<>::διάρκεια([]() {
		work(CRTPImplementation{});
		});
    std::cout << "Static (C.R.T.P.): " << sta_διάρκεια.count() << " msecs\n";
}
