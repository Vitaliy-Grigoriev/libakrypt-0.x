/* ----------------------------------------------------------------------------------------------- */
/*   Copyright (c) 2014 - 2017 by Axel Kenzo, axelkenzo@mail.ru                                    */
/*   All rights reserved.                                                                          */
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
/*   ak_tools.c                                                                                    */
/* ----------------------------------------------------------------------------------------------- */
 #include <ak_tools.h>
 #include <ak_buffer.h>
 #include <errno.h>

#ifdef LIBAKRYPT_HAVE_TERMIOS_H
 #include <termios.h>
#endif
#ifdef LIBAKRYPT_HAVE_FCNTL_H
  #include <fcntl.h>
#endif
#ifdef LIBAKRYPT_HAVE_SYSSTAT_H
 #include <sys/stat.h>
#endif
#ifdef LIBAKRYPT_HAVE_UNISTD_H
 #include <unistd.h>
#endif
#ifdef LIBAKRYPT_HAVE_ERRNO_H
 #include <errno.h>
#endif

/* ----------------------------------------------------------------------------------------------- */
/*! Функция рассматривает область памяти, на которую указывает указатель ptr, как массив
    последовательно записанных байт фиксированной длины, создает в оперативной памяти строку и
    последовательно выводит в нее значения, хранящиеся в заданной области памяти.
    Значения выводятся в шестнадцатеричной системе счисления.

    Пример использования.
  \code
    ak_uint8 data[5] = { 1, 2, 3, 4, 5 };
    ak_uint8 *str = ak_ptr_to_hexstr( data, 5, ak_false );
    printf("%s\n", str );
    free(str);
  \endcode

    @param ptr Указатель на область памяти
    @param ptr_size Размер области памяти (в байтах)
    @param reverse Последовательность вывода байт в строку. Если reverse равно \ref ak_false,
    то байты выводятся начиная с младшего к старшему.  Если reverse равно \ref ak_true, то байты
    выводятся начиная от старшего к младшему (такой способ вывода принят при стандартном выводе
    чисел: сначала старшие разряды, потом младшие).

    @return Функция возвращает указатель на созданную строку, которая должна быть позднее удалена
    пользователем с помощью вызова функции free(). В случае ошибки конвертации возвращается NULL.
    Код ошибки может быть получен с помощью вызова функции ak_error_get_code()                     */
/* ----------------------------------------------------------------------------------------------- */
 char *ak_ptr_to_hexstr( const ak_pointer ptr, const size_t ptr_size, const ak_bool reverse )
{
  char *nullstr = NULL;
  size_t len = 1 + (ptr_size << 1);

  if( ptr == NULL ) {
    ak_error_message( ak_error_null_pointer, __func__ , "using null pointer to data" );
    return NULL;
  }
  if( ptr_size <= 0 ) {
    ak_error_message( ak_error_zero_length, __func__ , "using data with zero or negative length" );
    return NULL;
  }

  if(( nullstr = (char *) malloc( len )) == NULL ) {
    ak_error_message( ak_error_out_of_memory, __func__ , "incorrect memory allocation" );
  }
    else {
      size_t idx = 0, js = 0, start = 0, offset = 2;
      ak_uint8 *data = ( ak_uint8 * ) ptr;

      memset( nullstr, 0, len );
      if( reverse ) { // движение в обратную сторону - от старшего байта к младшему
        start = len-3; offset = -2;
      }
      for( idx = 0, js = start; idx < ptr_size; idx++, js += offset ) {
        char str[4];
        ak_snprintf( str, 3, "%02X", data[idx] );
        memcpy( nullstr+js, str, 2 );
      }
    }
 return nullstr;
}

/* ----------------------------------------------------------------------------------------------- */
/*! Функция рассматривает область памяти, на которую указывает указатель ptr, как массив
    последовательно записанных байт фиксированной длины.
    Символьная (шестнадцатеричная) форма записи массива ptr помещается в заранее выделенный массив out.
    Если длины недостаточно, то возбуждается ошибка.

    Пример использования.
  \code
    ak_uint8 data[5] = { 1, 2, 3, 4, 5 };
    ak_uint8 data_out[12];

    if( ak_ptr_to_hexstr_static( data, 5, data_out, 12, ak_false ) == ak_error_ok )
      printf("%s\n", data_out );
  \endcode

    @param ptr Указатель на область памяти
    @param ptr_size Размер области памяти (в байтах)
    @param out Указатель на область памяти, в которую записывается символьное представление данных
    @param out_size Размер области памяти (в байтах); должен быть не менее, чем
    величина 1 + 2*`ptr_size`.

    @param reverse Последовательность вывода байт в строку. Если reverse равно \ref ak_false,
    то байты выводятся начиная с младшего к старшему.  Если reverse равно \ref ak_true, то байты
    выводятся начиная от старшего к младшему (такой способ вывода принят при стандартном выводе
    чисел: сначала старшие разряды, потом младшие).

    @return Если преобразование прошло успешно, возвращается \ref ak_error_ok. В противном случае
    возвращается код ошибки.                                                                       */
/* ----------------------------------------------------------------------------------------------- */
 int ak_ptr_to_hexstr_static( const ak_pointer ptr, const size_t ptr_size,
                                     ak_pointer out, const size_t out_size, const ak_bool reverse )
{
  ak_uint8 *data = ( ak_uint8 * ) ptr;
  size_t len = 1 + (ptr_size << 1);
  size_t idx = 0, js = 0, start = 0, offset = 2;

  if( ptr == NULL ) return ak_error_message( ak_error_null_pointer, __func__ ,
                                                                     "using null pointer to data" );
  if( ptr_size <= 0 ) return ak_error_message( ak_error_zero_length, __func__ ,
                                                        "using data with zero or negative length" );
  if( out_size < len ) return ak_error_message( ak_error_wrong_length, __func__ ,
                                                                 "using small size output buffer" );
  memset( out, 0, len );
  if( reverse ) { // движение в обратную сторону - от старшего байта к младшему
    start = len-3; offset = -2;
  }
  for( idx = 0, js = start; idx < ptr_size; idx++, js += offset ) {
     char str[4];
     ak_snprintf( str, 3, "%02X", data[idx] );
     memcpy( (ak_uint8 *)out+js, str, 2 );
  }

 return ak_error_ok;
}

/* ----------------------------------------------------------------------------------------------- */
/*! \brief Конвертация символа в целочисленное значение                                            */
/* ----------------------------------------------------------------------------------------------- */
 inline static ak_uint32 ak_xconvert( const char c )
{
    switch( c )
   {
      case 'a' :
      case 'A' : return 10;
      case 'b' :
      case 'B' : return 11;
      case 'c' :
      case 'C' : return 12;
      case 'd' :
      case 'D' : return 13;
      case 'e' :
      case 'E' : return 14;
      case 'f' :
      case 'F' : return 15;
      case '0' : return 0;
      case '1' : return 1;
      case '2' : return 2;
      case '3' : return 3;
      case '4' : return 4;
      case '5' : return 5;
      case '6' : return 6;
      case '7' : return 7;
      case '8' : return 8;
      case '9' : return 9;
      default : ak_error_set_value( ak_error_undefined_value ); return 0;
 }
}

/* ----------------------------------------------------------------------------------------------- */
/*! Функция преобразует строку символов, содержащую последовательность шестнадцатеричных цифр,
    в массив данных.

    @param hexstr Строка символов.
    @param ptr Указатель на область памяти (массив), в которую будут размещаться данные.
    @param size Максимальный размер памяти (в байтах), которая может быть помещена в массив.
    Если исходная строка требует больший размер, то возбуждается ошибка.
    @param reverse Последовательность считывания байт в память. Если reverse равно \ref ak_false
    то первые байты строки (с младшими индексами) помещаются в младшие адреса, а старшие байты -
    в старшие адреса памяти. Если reverse равно \ref ak_true, то производится разворот,
    то есть обратное преобразование при котором элементы строки со старшиси номерами помещаются
    в младшие разряды памяти (такое представление используется при считывании больших целых чисел).

    @return В случае успеха возвращается ноль. В противном случае, в частности,
                      когда длина строки превышает размер массива, возвращается код ошибки.        */
/* ----------------------------------------------------------------------------------------------- */
 int ak_hexstr_to_ptr( const char *hexstr, ak_pointer ptr, const size_t size, const ak_bool reverse )
{
  int i = 0;
  ak_uint8 *bdata = ptr;
  size_t len = 0;

  if( hexstr == NULL ) return ak_error_message( ak_error_null_pointer, __func__ ,
                                                             "using null pointer to a hex string" );
  if( ptr == NULL ) return ak_error_message( ak_error_null_pointer, __func__ ,
                                                                 "using null pointer to a buffer" );
  if( size == 0 ) return ak_error_message( ak_error_zero_length, __func__,
                                                          "using zero value for length of buffer" );
  len = strlen( hexstr );
  if( len&1 ) len++;
  len >>= 1;
  if( size < len ) return ak_error_message( ak_error_wrong_length, __func__ ,
                                                               "using a buffer with small length" );

  memset( ptr, 0, size ); // перед конвертацией мы обнуляем исходные данные
  ak_error_set_value( ak_error_ok );
  if( reverse ) {
    for( i = strlen( hexstr )-2, len = 0; i >= 0 ; i -= 2, len++ ) {
       bdata[len] = (ak_xconvert( hexstr[i] ) << 4) + ak_xconvert( hexstr[i+1] );
    }
    if( i == -1 ) bdata[len] = ak_xconvert( hexstr[0] );
  } else {
        for( i = 0, len = 0; i < (int) strlen( hexstr ); i += 2, len++ ) {
           bdata[len] = (ak_xconvert( hexstr[i] ) << 4);
           if( i < (int) strlen( hexstr )-1 ) bdata[len] += ak_xconvert( hexstr[i+1] );
        }
    }
 return ak_error_get_value();
}

/* ----------------------------------------------------------------------------------------------- */
/*! Функция сравнивает две области памяти одного размера, на которые указывают аргументы функции.
    Пример использования функции (результат выполнения функции должен быть \ref ak_false).
  \code
    ak_uint8 data_left[5] = { 1, 2, 3, 4, 5 };
    ak_uint8 data_right[5] = { 1, 2, 3, 4, 6 };

    if( ak_ptr_is_equal( data_left, data_right, 5 )) printf("Is equal");
     else printf("Not equal");
  \endcode

    @param left Указатель на область памяти, участвующей в сравнении слева.
    @param right Указатель на область пямяти, участвующей в сравнении справа.
    @param size Размер области, для которой производяится сравнение.
    @return Если данные идентичны, то возвращается \ref ak_true.
    В противном случае, а также в случае возникновения ошибки, возвращается \ref ak_false.
    Код шибки может быть получен с помощью выщова функции ak_error_get_value().                    */
/* ----------------------------------------------------------------------------------------------- */
 ak_bool ak_ptr_is_equal( const ak_pointer left, const ak_pointer right, const size_t size )
{
  size_t i = 0;
  ak_bool result = ak_true;
  ak_uint8 *lp = left, *rp = right;

  if(( left == NULL ) || ( right == NULL )) {
    ak_error_message( ak_error_null_pointer, __func__, "using a null pointer" );
    return ak_false;
  }

  for( i = 0; i < size; i++ )
     if( lp[i] != rp[i] ) result = ak_false;

  return result;
}

/* ----------------------------------------------------------------------------------------------- */
/*! @param ptr Данные, помещаемые в ASN1 структуру
    @param size Раззмер данных,помещаемых в ASN1 структуру (в байтах)
    @param ost Указатель на переменную типа OCTET_STRING_t, которая инициализируется данными,
    на которые указывает ptr.

    @return В случае успеха функция возвращает \ref ak_error_ok. В случае возникновения ошибки,
    возвращается ее код.                                                                           */
/* ----------------------------------------------------------------------------------------------- */
 int ak_ptr_to_asn1_octet_string( const ak_pointer ptr, const size_t size, OCTET_STRING_t *ost )
{
  if( ptr == NULL ) return ak_error_message( ak_error_null_pointer, __func__,
                                                               "using null pointer to source data");
  if( size == 0 ) return ak_error_message( ak_error_zero_length, __func__,
                                                                   "using zero length source data");
  if( ost == NULL ) return ak_error_message( ak_error_null_pointer, __func__,
                                                               "using null pointer OCTET_STRING_t");
  if(( ost->buf = malloc( size )) == NULL )
    return ak_error_message( ak_error_out_of_memory, __func__, "incorrect memory allocation");
  memcpy( ost->buf, ptr, size );
  ost->size = size;

 return ak_error_ok;
}

/* ----------------------------------------------------------------------------------------------- */
/*! Функция принимает два параметра:
    @param pass Строка, в которую будет помещен пароль. Память под данную строку должна быть
    выделена заранее. Если в данной памяти хранились какие-либо данные, то они будут уничтожены.
    @param psize Максимально возможная длина пароля. Предполагается, что именно
    это значение задает размер области памяти, на которую указывает pass.

    Отметим, что в случае ввода пароля нулевой длины функция возвращает ошибку с кодом
    \ref ak_error_terminal

    @return В случае успеха функция возвращает значение \ref ak_error_ok. В случае возникновения
    ошибки возвращается ее код.                                                                    */
/* ----------------------------------------------------------------------------------------------- */
 int ak_password_read( char *pass, const size_t psize )
{
   size_t len = 0;
   int error = ak_error_ok;

 #ifndef LIBAKRYPT_HAVE_TERMIOS_H
  #ifdef _WIN32
   char c = 0;
   DWORD mode, count;
   HANDLE ih = GetStdHandle( STD_INPUT_HANDLE  );
   if( !GetConsoleMode( ih, &mode ))
     return ak_error_message( ak_error_terminal, __func__, "not connected to a console" );
   SetConsoleMode( ih, mode & ~( ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT ));

   memset( pass, 0, psize );
   while( ReadConsoleA( ih, &c, 1, &count, NULL) && (c != '\r') && (c != '\n') && (len < psize-1) ) {
     pass[len]=c;
     len++;
   }
   pass[len]=0;

   /* восстанавливаем настройки консоли */
   SetConsoleMode( ih, mode );
   if(( len = strlen( pass )) < 1 )
     return ak_error_message( ak_error_zero_length, __func__ , "input a very short password");
   return error;

  #endif
   return ak_error_undefined_function;

 #else
  /* обрабатываем терминал */
   struct termios ts, ots;

   tcgetattr( STDIN_FILENO, &ts);   /* получаем настройки терминала */
   ots = ts;
   ts.c_cc[ VTIME ] = 0;
   ts.c_cc[ VMIN  ] = 1;
   ts.c_iflag &= ~( BRKINT | INLCR | ISTRIP | IXOFF ); // ICRNL | IUTF8
   ts.c_iflag |=    IGNBRK;
   ts.c_oflag &= ~( OPOST );
   ts.c_cflag &= ~( CSIZE | PARENB);
   ts.c_cflag |=    CS8;
   ts.c_lflag &= ~( ECHO | ICANON | IEXTEN | ISIG );
   tcsetattr( STDIN_FILENO, TCSAFLUSH, &ts );
   tcgetattr( STDIN_FILENO, &ts ); /* проверяем, что все установилось */
   if( ts.c_lflag & ECHO ) {
        ak_error_message( error = ak_error_terminal, __func__, "failed to turn off echo" );
        goto lab_exit;
   }

   memset( pass, 0, psize );
   fgets( pass, psize, stdin );
   if(( len = strlen( pass )) < 2 )
     ak_error_message( error = ak_error_zero_length, __func__ , "input a very short password");
   if( len > 0 ) pass[len-1] = 0;
    else pass[0] = 0;

  /* убираем за собой и восстанавливаем настройки */
   lab_exit: tcsetattr( STDIN_FILENO, TCSANOW, &ots );
   return error;
 #endif

 /* некорректный путь компиляции исходного текста функции */
 return ak_error_undefined_function;
}

/* ----------------------------------------------------------------------------------------------- */
/*! @param buffer Буфер, в который помещается пароль. Буфер должен быть создан заранее с заданным
    размером хранящихся в нем данных (с помощью вызова функции ak_buffer_new_size() )
    Если в буффере хранились данные, то они будут уничтожены.

    Отметим, что в случае ввода пароля нулевой длины функция возвращает ошибку с кодом
    \ref ak_error_terminal

    @return В случае успеха функция возвращает значение \ref ak_error_ok. В случае возникновения
    ошибки возвращается ее код.                                                                    */
/* ----------------------------------------------------------------------------------------------- */
 int ak_password_read_buffer( ak_buffer password )
{
  int error = ak_error_ok;
  if( password == NULL ) return ak_error_message( ak_error_null_pointer, __func__,
                                                       "using null pointer to password buffer" );
  if( password->data == NULL ) return ak_error_message( ak_error_zero_length, __func__,
                                            "using a password buffer with null internal array" );
  if( password->size == 0 ) return ak_error_message( ak_error_zero_length, __func__,
                                                    "using a password buffer with zero length" );
  if(( error = ak_password_read( password->data, password->size )) != ak_error_ok ) {
    return ak_error_message( error, __func__, "invalind password reading from standard console");
  }
 return ak_error_ok;
}

/* ----------------------------------------------------------------------------------------------- */
 int ak_file_read_by_lines( const char *filename, ak_file_read_function *function , ak_pointer ptr )
{
  #define buffer_length ( FILENAME_MAX + 160 )

  char ch;
  struct stat st;
  size_t idx = 0, off = 0;
  int fd = 0, error = ak_error_ok;
  char localbuffer[buffer_length];

 /* проверяем наличие файла и прав доступа к нему */
  if(( fd = open( filename, O_RDONLY | O_BINARY )) < 0 )
    return ak_error_message_fmt( ak_error_open_file,
                             __func__, "wrong open file \"%s\" - %s", filename, strerror( errno ));
  if( fstat( fd, &st ) ) {
    close( fd );
    return ak_error_message_fmt( ak_error_access_file, __func__ ,
                              "wrong stat file \"%s\" with error %s", filename, strerror( errno ));
  }

 /* нарезаем входные на строки длиной не более чем buffer_length - 2 символа */
  memset( localbuffer, 0, buffer_length );
  for( idx = 0; idx < (size_t) st.st_size; idx++ ) {
     if( read( fd, &ch, 1 ) != 1 ) {
       close(fd);
       return ak_error_message_fmt( ak_error_read_data, __func__ ,
                                                                "unexpected end of %s", filename );
     }
     if( off > buffer_length - 2 ) {
       close( fd );
       return ak_error_message_fmt( ak_error_read_data, __func__ ,
                          "%s has a line with more than %d symbols", filename, buffer_length - 2 );
     }
    if( ch == '\n' ) {
      #ifdef _WIN32
       if( off ) localbuffer[off-1] = 0;  /* удаляем второй символ перехода на новую строку */
      #endif
      function( localbuffer, ptr );
     /* далее мы очищаем строку независимо от ее содержимого */
      off = 0;
      memset( localbuffer, 0, buffer_length );
    } else localbuffer[off++] = ch;
  }

  close( fd );
 return error;
}

/* ----------------------------------------------------------------------------------------------- */
/*                               Функции для работы с asn1 структурами                             */
/* ----------------------------------------------------------------------------------------------- */
/*! Функция освобождает память, созданную в процессе преобразования секретного ключа
    в структуру `SecretKeyData`. Функция должна использоваться для статически выделенной структуры.
    В случае динамического выделения структуры `SecretKeyData` с помощью malloc()
    целесообразнее использовать вызов

    \code
      asn_DEF_SecretKeyData.free_struct( &asn_DEF_SecretKeyData, secretKeyData, 0);
    \endcode

    @param secretKeyData ссылка на статическую структуру типа SecretKeyData_t.

    @return В случае успеха функция возвращает \ref ak_error_ok. В противном случае
    возвращается код ошибки.                                                                       */
/* ----------------------------------------------------------------------------------------------- */
 int ak_asn1_secret_key_data_destroy( SecretKeyData_t *secretKeyData )
{
  if( secretKeyData == NULL ) return ak_error_message( ak_error_null_pointer, __func__,
                                    "using null pointer to asn1 secretKeyData structure" );

 /* освобождаем память */
  if( secretKeyData->data.value.buf != NULL ) free( secretKeyData->data.value.buf );
  if( secretKeyData->data.engine.buf != NULL ) free( secretKeyData->data.engine.buf );
  if( secretKeyData->data.number.buf != NULL ) free( secretKeyData->data.number.buf );

  if( secretKeyData->data.parameters.encryptionSalt.buf != NULL )
                                              free( secretKeyData->data.parameters.encryptionSalt.buf );
  if( secretKeyData->data.parameters.encryptionMode.algorithm.buf != NULL )
                                    free( secretKeyData->data.parameters.encryptionMode.algorithm.buf );
  if( secretKeyData->data.parameters.encryptionMode.parameters != NULL )
   asn_DEF_ANY.free_struct( &asn_DEF_ANY, secretKeyData->data.parameters.encryptionMode.parameters, 0 );
  if( secretKeyData->data.parameters.encryptionIV.buf != NULL )
                                                free( secretKeyData->data.parameters.encryptionIV.buf );

  if( secretKeyData->data.parameters.integritySalt.buf != NULL )
                                               free( secretKeyData->data.parameters.integritySalt.buf );
  if( secretKeyData->data.parameters.integrityMode.algorithm.buf != NULL )
                                     free( secretKeyData->data.parameters.integrityMode.algorithm.buf );
  if( secretKeyData->data.parameters.integrityMode.parameters != NULL )
    asn_DEF_ANY.free_struct( &asn_DEF_ANY, secretKeyData->data.parameters.integrityMode.parameters, 0 );
  if(  secretKeyData->data.parameters.integrityIV != NULL )
    asn_DEF_OCTET_STRING.free_struct( &asn_DEF_OCTET_STRING,
                                                        secretKeyData->data.parameters.integrityIV, 0 );
  if( secretKeyData->data.resource != NULL )
    asn_DEF_KeyResource.free_struct( &asn_DEF_KeyResource, secretKeyData->data.resource, 0 );
  if( secretKeyData->data.description != NULL )
    asn_DEF_OCTET_STRING.free_struct( &asn_DEF_OCTET_STRING, secretKeyData->data.description, 0 );

  if( secretKeyData->integrityCode.buf != NULL ) free( secretKeyData->integrityCode.buf );

 /* обнуляем указатели */
  memset( secretKeyData, 0, sizeof( struct SecretKeyData ));

 return ak_error_ok;
}

/* ----------------------------------------------------------------------------------------------- */
/*! \brief Функция для итерационной записи данных, получаемых при кодировании asn1 структуры. */
 static int ak_asn1_save_write_out( const void *buffer, size_t size, void *app_key )
{
  int *fd = app_key;
  ssize_t wrote = write( *fd, buffer, size );
  return ( wrote == size ) ? 0 : -1;
}

/* ----------------------------------------------------------------------------------------------- */
 int ak_asn1_save_to_der_file( struct asn_TYPE_descriptor_s *type_descriptor,
                                                           void *struct_ptr, const char *filename )
{
 struct stat st;
 asn_enc_rval_t ec;
 int fd = 0, error = ak_error_ok;

  if( filename == NULL ) return ak_error_message( ak_error_null_pointer, __func__ ,
                                                                "use a null pointer to filename" );
 /* проверяем, что файл не существует */
  if(( fd = open( filename, O_RDONLY | O_BINARY )) != -1 ) {
   close( fd );
   return ak_error_message_fmt( ak_error_file_exists, __func__,
                                                          "file \"%s\" already exists", filename );
 }

 /* создаем файл */
  if(( fd = creat( filename, S_IRUSR|S_IWUSR )) < 0 )
    return ak_error_message( ak_error_open_file, __func__, strerror( errno ));

  if( fstat( fd, &st ) != 0 ) {
    close( fd );
    return ak_error_message( ak_error_access_file,  __func__, strerror( errno ));
  }

  ec = der_encode( type_descriptor, struct_ptr, ak_asn1_save_write_out, &fd );
  if( ec.encoded < 0 )
    ak_error_message_fmt( error = ak_error_wrong_asn1_encode, __func__,
      "incorrect encoding asn1 structure near %s type", ec.failed_type->name );

  close(fd);
 return error;
}

/* ----------------------------------------------------------------------------------------------- */
/*                                                                                     ak_tools.c  */
/* ----------------------------------------------------------------------------------------------- */
