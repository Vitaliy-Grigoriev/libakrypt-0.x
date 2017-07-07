/* ----------------------------------------------------------------------------------------------- */
/*  Copyright (c) 2017 by Axel Kenzo, axelkenzo@mail.ru                                            */
/*                                                                                                 */
/*  Разрешается повторное распространение и использование как в виде исходного кода, так и         */
/*  в двоичной форме, с изменениями или без, при соблюдении следующих условий:                     */
/*                                                                                                 */
/*   1. При повторном распространении исходного кода должно оставаться указанное выше уведомление  */
/*      об авторском праве, этот список условий и последующий отказ от гарантий.                   */
/*   2. При повторном распространении двоичного кода должна сохраняться указанная выше информация  */
/*      об авторском праве, этот список условий и последующий отказ от гарантий в документации     */
/*      и/или в других материалах, поставляемых при распространении.                               */
/*   3. Ни имя владельца авторских прав, ни имена его соратников не могут быть использованы в      */
/*      качестве рекламы или средства продвижения продуктов, основанных на этом ПО без             */
/*      предварительного письменного разрешения.                                                   */
/*                                                                                                 */
/*  ЭТА ПРОГРАММА ПРЕДОСТАВЛЕНА ВЛАДЕЛЬЦАМИ АВТОРСКИХ ПРАВ И/ИЛИ ДРУГИМИ СТОРОНАМИ "КАК ОНА ЕСТЬ"  */
/*  БЕЗ КАКОГО-ЛИБО ВИДА ГАРАНТИЙ, ВЫРАЖЕННЫХ ЯВНО ИЛИ ПОДРАЗУМЕВАЕМЫХ, ВКЛЮЧАЯ, НО НЕ             */
/*  ОГРАНИЧИВАЯСЬ ИМИ, ПОДРАЗУМЕВАЕМЫЕ ГАРАНТИИ КОММЕРЧЕСКОЙ ЦЕННОСТИ И ПРИГОДНОСТИ ДЛЯ КОНКРЕТНОЙ */
/*  ЦЕЛИ. НИ В КОЕМ СЛУЧАЕ НИ ОДИН ВЛАДЕЛЕЦ АВТОРСКИХ ПРАВ И НИ ОДНО ДРУГОЕ ЛИЦО, КОТОРОЕ МОЖЕТ    */
/*  ИЗМЕНЯТЬ И/ИЛИ ПОВТОРНО РАСПРОСТРАНЯТЬ ПРОГРАММУ, КАК БЫЛО СКАЗАНО ВЫШЕ, НЕ НЕСЁТ              */
/*  ОТВЕТСТВЕННОСТИ, ВКЛЮЧАЯ ЛЮБЫЕ ОБЩИЕ, СЛУЧАЙНЫЕ, СПЕЦИАЛЬНЫЕ ИЛИ ПОСЛЕДОВАВШИЕ УБЫТКИ,         */
/*  ВСЛЕДСТВИЕ ИСПОЛЬЗОВАНИЯ ИЛИ НЕВОЗМОЖНОСТИ ИСПОЛЬЗОВАНИЯ ПРОГРАММЫ (ВКЛЮЧАЯ, НО НЕ             */
/*  ОГРАНИЧИВАЯСЬ ПОТЕРЕЙ ДАННЫХ, ИЛИ ДАННЫМИ, СТАВШИМИ НЕПРАВИЛЬНЫМИ, ИЛИ ПОТЕРЯМИ ПРИНЕСЕННЫМИ   */
/*  ИЗ-ЗА ВАС ИЛИ ТРЕТЬИХ ЛИЦ, ИЛИ ОТКАЗОМ ПРОГРАММЫ РАБОТАТЬ СОВМЕСТНО С ДРУГИМИ ПРОГРАММАМИ),    */
/*  ДАЖЕ ЕСЛИ ТАКОЙ ВЛАДЕЛЕЦ ИЛИ ДРУГОЕ ЛИЦО БЫЛИ ИЗВЕЩЕНЫ О ВОЗМОЖНОСТИ ТАКИХ УБЫТКОВ.            */
/*                                                                                                 */
/*   ak_context_manager.c                                                                          */
/* ----------------------------------------------------------------------------------------------- */
 #include <ak_context_manager.h>

/* ----------------------------------------------------------------------------------------------- */
/*! \brief Мьютекс для блокировки структуры управления контекстами */
 static pthread_mutex_t ak_context_manager_mutex = PTHREAD_MUTEX_INITIALIZER;

/* ----------------------------------------------------------------------------------------------- */
/*                                    класс ak_context_manager                                     */
/* ----------------------------------------------------------------------------------------------- */
/*! Функция инициализирует структуру управления контекстами, присваивая ее полям значения,
    необходимые для обеспечения корректной работы.
    Начальное значение ожидаемого структурой количества контекстов, с которыми будет произодится
    работа, является внешним параметром библиотеки. Данное значение устанавливается
    в файле \ref libakrypt.conf

    @param manager Указатель на структуру управления ключами
    @return В случае успеха функция возвращает \ref ak_error_ok (ноль). В противном случае
    возвращается код ошибки.                                                                       */
/* ----------------------------------------------------------------------------------------------- */
 int ak_context_manager_create( ak_context_manager manager )
{
  size_t idx = 0;
  int error = ak_error_ok;

  if( manager == NULL ) return ak_error_message( ak_error_null_pointer, __func__ ,
                                            "using a null pointer to context manager structure" );
 /* инициализируем генератор ключей */
#ifdef __linux__
  if(( error = ak_random_create_file( &manager->key_generator, "/dev/random" )) != ak_error_ok )
    return ak_error_message( error, __func__,
                             "wrong initialization of /dev/random for random number generation" );
#else
 #ifdef _WIN32
   if(( error = ak_random_create_winrtl( &manager->key_generator )) != ak_error_ok )
     return ak_error_message( error, __func__,
                           __func__, "wrong initialization of crypto provider random generator" );
 #else
   #error Using a not defined path of compilation
 #endif
#endif

 /* инициализируем указатели контекстов */
  manager->size = 4;
  ak_error_message( ak_error_ok, __func__ , "TODO: load manager->size value from /etc/libakrypt.conf");

  if(( manager->array = malloc( manager->size*sizeof( ak_pointer ))) == NULL ) {
    ak_context_manager_destroy( manager );
    return ak_error_message( ak_error_out_of_memory, __func__ ,
                                            "wrong memory allocation for context manager nodes" );
  }
  for( idx = 0; idx < manager->size; idx++ ) manager->array[idx] = NULL;

 return ak_error_ok;
}

/* ----------------------------------------------------------------------------------------------- */
/*! Функция выделяет дополнительную память под массив хранения контекстов.
    При выделении памяти производится перенос содержащихся в массиве указателей
    (элементов структуры управления контекстами) в новую область памяти.
    Старая область памяти уничтожается.

    При выделении памяти размер новой области увеличивается в два раза, по сравнению с предыдущим
    объемом, то есть происходит двукратное увеличение. Максимальное число
    хранимых в структуре управления контекстов является внешним параметром библиотеки.
    Данное значение устанавливается в файле \ref libakrypt.conf

    @param manager Указатель на структуру управления ключами
    @return В случае успеха функция возвращает \ref ak_error_ok (ноль). В противном случае
    возвращается код ошибки.                                                                       */
/* ----------------------------------------------------------------------------------------------- */
 int ak_context_manager_morealloc( ak_context_manager manager )
{
  ak_context_node *newarray = NULL;
  size_t idx = 0, newsize = (manager->size << 1);

  if( manager == NULL ) return ak_error_message( ak_error_null_pointer, __func__ ,
                                            "using a null pointer to context manager structure" );
  if( newsize <= manager->size )
    return ak_error_message( ak_error_context_manager_size, __func__ ,
                                      "unexpected value of new value of context manager's size" );
  if( newsize > 4096 ) return ak_error_message( ak_error_context_manager_max_size, __func__,
                                   "current size of context manager exceeds permissible bounds" );
    ak_error_message( ak_error_ok, __func__ ,
                            "TODO: load maximum of manager->size value from /etc/libakrypt.conf");

  if(( newarray = malloc( newsize*sizeof( ak_pointer ))) == NULL )
    return ak_error_message( ak_error_out_of_memory, __func__ ,
                                              "wrong memory allocation for context manager nodes" );

 /* копируем данные и очищаем память */
  for( idx = 0; idx < manager->size; idx++ ) {
     newarray[idx] = manager->array[idx];
     manager->array[idx] = NULL;
   }
  for( idx = manager->size; idx < newsize; idx++ ) newarray[idx] = NULL;

  free( manager->array );
  manager->array = newarray;
  manager->size = newsize;

 return ak_error_ok;
}

/* ----------------------------------------------------------------------------------------------- */
/*! Функция удаляет структуру управления контекстами, уничтожая данные, которыми она владеет.
    При выполнении функции:
    - уничтожаются контексты, хранящиеся в структуре,
    - уничтожается генератор псевдо-случайных чисел, использовавшийся для генерации
      ключевой информации.

    @param manager Указатель на структуру управления ключами
    @return В случае успеха функция возвращает \ref ak_error_ok (ноль). В противном случае
    возвращается код ошибки.                                                                       */
/* ----------------------------------------------------------------------------------------------- */
 int ak_context_manager_destroy( ak_context_manager manager )
{
  size_t idx = 0;
  int error = ak_error_ok;

  if( manager == NULL ) return ak_error_message( ak_error_null_pointer, __func__ ,
                                            "using a null pointer to context manager structure" );
  if( manager->array == NULL ) {
    ak_error_message( error = ak_error_undefined_value, __func__ ,
                                                   "cleaning context manager with empty memory" );
  } else {
          /* удаляем ключевые структуры */
           for( idx = 0; idx < manager->size; idx++ )
              if( manager->array[idx] != NULL )
                manager->array[idx] = ak_context_node_delete( manager->array[idx] );

           /* очищаем и уничтожаем память */
           memset( manager->array, 0, manager->size);
           free( manager->array );
           manager->array = NULL;
  }
  manager->size = 0;

 /* удаляем генератор ключей */
  if(( error = ak_random_destroy( &manager->key_generator )) != ak_error_ok )
    return ak_error_message( error, __func__ , "using a null pointer to random key generator" );

 return error;
}

/* ----------------------------------------------------------------------------------------------- */
/*! Функция находит первый равный NULL адрес элемента структуры и
    помещает по этому адресу новый элемент. В случае, если текущей объем памяти недостаточен для
    размещения нового элемента, происходит выделение нового фрагмента памяти.

    @param manager Указатель на структуру управления ключами
    @param ctx Контекст, который будет храниться в структуре управленияя контекстами
    @param engine тип контекста: блочный шифр, функия хеширования, массив с данными и т.п.
    @param description пользовательское описание контекста
    @param func функция освобождения памяти, занимаемой контекстом
    @return Функция возвращает идентификатор созданного контекста. В случае
    возникновения ошибки возвращается значение \ref ak_key_wrong. Код ошибки может быть получен
    с помощью вызова функции ak_error_get_value().                                                 */
/* ----------------------------------------------------------------------------------------------- */
 ak_handle ak_context_manager_add_node( ak_context_manager manager, ak_pointer ctx,
                     ak_oid_engine engine, const char *description, ak_function_free_object *func )
{
  size_t idx = 0;
  int error = ak_error_ok;
  ak_context_node node = NULL;

 /* минимальные проверки */
  if( manager == NULL ) {
    ak_error_message( ak_error_null_pointer, __func__, "using a null pointer to context manager" );
    return ak_error_wrong_handle;
  }
  if( ctx == NULL ) {
    ak_error_message( ak_error_null_pointer, __func__, "using a null pointer to context" );
    return ak_error_wrong_handle;
  }

 /* блокируем доступ к структуре управления контекстами */
  pthread_mutex_lock( &ak_context_manager_mutex );

 /* ищем свободный адрес */
  for( idx = 0; idx < manager->size; idx++ ) {
     if( manager->array[idx] == NULL ) break;
  }
  if( idx == manager->size ) {
    if(( error =  ak_context_manager_morealloc( manager )) != ak_error_ok ) {
      ak_error_message( error, __func__, "wrong creation of context descriptor" );
      pthread_mutex_unlock( &ak_context_manager_mutex );
      return ak_error_wrong_handle;
    }
  }

 /* адрес найден, теперь размещаем контекст */
  if(( node = ak_context_node_new( ctx, idx, engine, description, func )) == NULL ) {
    ak_error_message( ak_error_get_value(), __func__, "wrong creation of context manager node" );
    pthread_mutex_unlock( &ak_context_manager_mutex );
    return ak_error_wrong_handle;
  }
  manager->array[idx] = node;
  pthread_mutex_unlock( &ak_context_manager_mutex );

 return idx;
}

/* ----------------------------------------------------------------------------------------------- */
 int ak_context_manager_delete_node( ak_context_manager manager, ak_handle idx )
{
  if( manager == NULL ) return ak_error_message( ak_error_null_pointer, __func__,
                                                      "using a null pointer to context manager" );
  if(( idx < 0 ) || ( idx >= manager->size ))
    return ak_error_message( ak_error_wrong_handle, __func__,
                                                      "using an unexpected value of secret key" );

 /* блокируем доступ и изменяем поля структуры управления контекстами */
  pthread_mutex_lock( &ak_context_manager_mutex );
  manager->array[idx] = ak_context_node_delete( manager->array[idx] );
  pthread_mutex_unlock( &ak_context_manager_mutex );

 return ak_error_ok;
}

/* ----------------------------------------------------------------------------------------------- */
/*                                 класс ak_context_manager_node                                   */
/* ----------------------------------------------------------------------------------------------- */
/*! Функция создает новый элемент структуры управления контекстами, заполняя его поля значениями,
    передаваемыми в качестве аргументов функции.

    @param ctx Контекст, который будет храниться в структуре управленияя контекстами
    @param id Идентификатор контекста, величина по которой пользовать может получить доступ
    к функциям, реализующим действия с контекстом.
    @param engine тип контекста: блочный шифр, функия хеширования, массив с данными и т.п.
    @param description пользовательское описание контекста
    @param func функция освоюождения памяти, занимаемой контекстом
    @return Функция возвращает указатель на созданный элемент структуры управления контекстами.
    В случае возникновения ошибки возвращается NULL. Код ошибки может быть получен с помощью
    вызова функции ak_error_get_value().                                                           */
/* ----------------------------------------------------------------------------------------------- */
 ak_context_node ak_context_node_new( ak_pointer ctx, ak_handle id, ak_oid_engine engine,
                                            const char *description, ak_function_free_object *func )
{
  int error = ak_error_ok;
  ak_context_node node = NULL;

 /* минимальные проверки */
  if( ctx == NULL ) {
    ak_error_message( ak_error_null_pointer, __func__, "using a null pointer to context" );
    return NULL;
  }
  if( func == NULL ) {
    ak_error_message( ak_error_undefined_function, __func__,
                                         "using a null pointer to context free function" );
    return NULL;
  }

 /* создаем контекст */
  if(( node = malloc( sizeof( struct context_node ))) == NULL ) {
    ak_error_message( ak_error_out_of_memory, __func__,
                                  "wrong memory allocation for new context manager node" );
    return NULL;
  }

 /* инициализируем буффер и присваиваем ему значение */
  if(( error = ak_buffer_create( &node->description )) != ak_error_ok ) {
    ak_error_message( error, __func__, "wrong creation of internal buffer" );
    return ( node = ak_context_node_delete( node ));
  }

  if(( error = ak_buffer_set_str( &node->description, description )) != ak_error_ok ) {
    ak_error_message( error, __func__, "wrong initialization of internal buffer" );
    return ( node = ak_context_node_delete( node ));
  }

 /* присваиваем остальные данные */
  node->ctx = ctx;
  node->id = id;
  node->engine = engine;
  node->free = func;
  node->status = node_is_equal;

 return node;
}

/* ----------------------------------------------------------------------------------------------- */
/*! @param pointer указатель на элемент структуры управления контекстами.
    @return Функция всегда возвращает NULL.                                                        */
/* ----------------------------------------------------------------------------------------------- */
 ak_pointer ak_context_node_delete( ak_pointer pointer )
{
  ak_context_node node = ( ak_context_node ) pointer;

  if( node == NULL ) {
    ak_error_message( ak_error_null_pointer, __func__,
                                       "wrong deleting a null pointer to context manager node" );
    return NULL;
  }

  ak_buffer_destroy( &node->description );

  if( node->free != NULL ) {
    if( node->ctx != NULL ) node->ctx = node->free( node->ctx );
    node->free = NULL;
  }
  node->id = ak_error_wrong_handle;
  node->status = node_undefined;
  node->engine = undefined_engine;
  free( node );

 return NULL;
}

/* ----------------------------------------------------------------------------------------------- */
/*! \example example-context-manager.c                                                             */
/* ----------------------------------------------------------------------------------------------- */
/*                                                                           ak_context_manager.c  */
/* ----------------------------------------------------------------------------------------------- */