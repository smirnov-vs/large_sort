# External sort util

Сборка:

`
mkdir build && cd build && cmake .. && make -j
`

Сгенерировать строки:

`
./generator -n 50000000 -m 100 > data
`

Запишет в data 50 млн строк, каждая не больше 100 символов.

Отсортировать строки:

`
./sort -o data.sorted -b 2000000 -f 30 -d . < data
`

или

`
./sort -i data -o data.sorted -b 2000000 -f 30 -d .
`

Сортирует входной файл data, читая по 2 млн строк в память за раз.
Затем сортирует эти строки и записывает во временный файл.
Временных файлов может быть не больше чем в параметре `-f`.
При достижении максимума временных файлов или конце входных данных временные файлы мержатся в один общий.
Если временные файлы мержатся несколько раз вместе с ними мержится и результат предыдущего слияния.
Параметр `-d` задает путь к директории для временных файлов.

Так как скорость мержа при увеличении размера результирующего файла деградирует, оптимальнее всего свести количество мержей к минимуму с потерей места на диске вплоть до 2х размеров исходного файла дополнительно к его размеру.