Учебный пример. Решение классической задачи об обедающих философах. Два подхода:

1. на основе std::mutex
2. на основе атомарных операций


Программа намеренно спроектирована так, чтобы возник объект-одиночка, используемый в нескольких потоках одновременно. 
Показан один из способов создать thread-safe singleton.
