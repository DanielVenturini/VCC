/// dados que serao usados para processar os tokens
enum TokenType;
struct TokenRecord;
char currentCharacter
File file;

int openFile(char *filename);   /// realiza as operacoes de abertura do arquivo
char* getCharacter();           /// le um unico caracter do arquivo ou retorna um que ja foi lido mas nao processado
TokenType getToken(void);       /// retorna um token
