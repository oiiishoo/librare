# librare
Custom datastructures and personal utilities c/c++ implementation

25.01.25 думал что листы с единой ссылкой лучше чем двусыльные, это оказалось ошибкой 

8.10.25 MTqueue 10m iops test 3reader3writer 10m batch

27.11.2025 trie структурная переделка, залив скрапа в соответствующий файл scrap_trie.h для публичного доступа. Так Далее -

Добавка компонента udpcomponent для более простой работы клиента и сервера на основе udp winsock.

Залив timerpool компонента на основе busywait windows QPC.

Залив компонента tcp, упрощает инит и юз

Идея о создании Trie, была отброшена в связи с  не релевантностью тяжёлой адаптивной реализации
