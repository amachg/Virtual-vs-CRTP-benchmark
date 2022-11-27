//Inheritance: Virtual vs. CRTP ταχύτητα

#include <iostream>
#include <chrono>

const unsigned N = 200'000;

///////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////

//The alternative Statically-polymorphic

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

// Timing

template <typename TimeUnit = std::chrono::milliseconds>
struct Timer {
	template<typename Functor, typename ...Args>
	static auto duration(Functor&& functor, Args&&... args) {
		// fire Timer
		const auto before = std::chrono::high_resolution_clock::now();
		std::invoke(std::forward<Functor>(functor), std::forward<Args>(args)...);
		// stop Timer
		const auto now = std::chrono::high_resolution_clock::now();
		return std::chrono::duration_cast<TimeUnit>(now - before);
	}
};

int main() {
    std::cout << "Started timing polymorphic modes in c++ \n";
	
	const auto staticDuration = Timer<>::duration([]() {
		work(CRTPImplementation{});
		});
    std::cout << "Static (CRTP): " << staticDuration.count() << " msecs\n";

	const auto dynamicDuration = Timer<>::duration([]() {
		work(DynamicImplementation{});
		});
    std::cout << "Dynamic (Virtuals): " << dynamicDuration.count() << " msecs\n";
}
