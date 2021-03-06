lexer grammar TLexer;

COMMA : ',';
SEMICOLON : ';';

END : 'end';

// условия
IF: 'if';
ELSE : 'else';

// цикл
WHILE : 'while';

// булевые переменные
TRUE : 'true';
FALSE : 'false';

// арифметические операции
PLUS : '+';
MINUS : '-';
MUL : '*';
DIV : '/';

// сравнения
EQUAL : '==';
NOT_EQUAL : '!=';
GREATER : '>';
LESS : '<';
LESS_EQUAL : '<=';
GREATER_EQUAL : '>=';

// присваивание
ASSIGN : '=';

// скобки
L_BRACKET : '(';
R_BRACKET : ')';
L_SQBRACKET : '[';
R_SQBRACKET : ']';

// операция ввода-вывода
UGETS : 'ugets';    // беззнаковые целые
IGETS : 'igets';    // целые
FGETS : 'fgets';    // дробные
PUTS : 'puts';

// перевод каретки и новая строка
NEWLINE : ('\r')? '\n';

WS : (' ' | '\t')+ -> skip;

// комменты
COMMENT : ('#' ~('\r' | '\n')* NEWLINE) -> skip;
E_COMMENT : ('=begin' .*? '=end') -> skip;
// отступы (пробелы и табы)

UNDERSCORE : '_';
// буквы
fragment SMALL : [a-z];
fragment BIG : [A-Z];
// числа
fragment DIGIT : [0-9];
INT : (DIGIT UNDERSCORE?)+;
EXPONENT : ('e' | 'E') (PLUS | MINUS)? INT;
FLOAT : INT '.' INT (EXPONENT)?
      | INT EXPONENT
      ;
// идентификатор
ID : (SMALL | BIG | UNDERSCORE)(SMALL | BIG | UNDERSCORE | DIGIT)*;