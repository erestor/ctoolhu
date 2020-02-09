Ctoolhu
=======

What is it?

- C++ library featuring general tools and lightweight implementations of some design patterns
- using modern C++, <a href="http://boost.org">Boost</a> & <a href="http://loki-lib.sourceforge.net/">Loki</a>, platform-independent
- compatible with <a href="https://github.com/emscripten-core/emscripten">Emscripten</a> for building WebAssembly

Where is it?

- Ctoolhu is located in ctoolhu directory
- the other files in the repository make it possible for Ctoolhu to be opened in Visual Studio

What does it give you?

- event
  - event aggregator with auto-subscription
- maths
  - epsilon-based floating point comparison
- property_tree
  - simplifies JSON conversion with boost::property_tree
- random
  - random number generators
  - random selector for containers
- singleton
  - singleton holder with variable lifetime (esp. for Emscripten builds)
- std_ext
  - simplifies typical usage of standard library algorithms
- thread
  - locking proxy for object-level locking
  - implementation of async using a thread pool (esp. for Emscripten builds)
- time
  - stopwatch for duration measurement
- typesafe
  - type-safe integers

What next?

- add more components as they come up in production

Who uses it?

- <a href="https://skolaris.net">Skolaris: Online school timetable software</a>

Licence

- MIT
