CREATE TABLE IF NOT EXISTS _Teacher (
        _EMAIL TEXT PRIMARY KEY, 
        _FIO text,
        _PASS text
      );

CREATE TABLE IF NOT EXISTS _Group (
	_ID bigserial PRIMARY KEY,
        _NAME VARCHAR,
        _EMAIL_TEACHER text,
        CONSTRAINT zakrep FOREIGN KEY (_EMAIL_TEACHER) REFERENCES _Teacher (_EMAIL)
      );
      
CREATE TABLE IF NOT EXISTS _Student (
        _FIO text,
        _EMAIL TEXT PRIMARY KEY,
        _PASS text,
        _NUMBER integer,
        _GROUP bigserial,
        CONSTRAINT sostoit FOREIGN KEY(_GROUP) REFERENCES _Group(_ID)
      );
      
CREATE TABLE IF NOT EXISTS _Quest (
        _ID bigserial PRIMARY KEY,
        _NAME text,
        _TEXT text,
        _VARIANTS text,
        _ANSWER text
      );  

CREATE TABLE IF NOT EXISTS _Img (
	_ID bigserial PRIMARY KEY,
	_SRC text,
	_TEXT text,
	_QUEST bigserial,
	CONSTRAINT prin FOREIGN KEY (_QUEST) REFERENCES _Quest(_ID)
);

CREATE TABLE IF NOT EXISTS _Test (
	_ID bigserial PRIMARY KEY,
	_NAME text
);

CREATE TABLE IF NOT EXISTS _SETUP(
	_QUEST bigserial REFERENCES _Quest (_ID) ON UPDATE CASCADE ON DELETE CASCADE,
	_TEST bigserial REFERENCES _Test (_ID) ON UPDATE CASCADE ON DELETE CASCADE,
	_NUMBER integer,
	CONSTRAINT _KEYS PRIMARY KEY (_QUEST, _TEST)
);

CREATE TABLE IF NOT EXISTS _SavedTest(
	_ID bigserial PRIMARY KEY,
	_TEST bigserial,
	_ANSWERS text,
	_UNCHECKED_ANSWERS text,
	_EMAIL_STUDENT text,  
	_SCORE integer,
	_date timestamp,
	CONSTRAINT sdelal FOREIGN KEY(_EMAIL_STUDENT) REFERENCES _Student(_EMAIL),
	CONSTRAINT est FOREIGN KEY(_TEST) REFERENCES _TEST(_ID)
);



