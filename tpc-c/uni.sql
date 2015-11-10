create table Studenten
(
  MatrNr   integer not null,
  Name     varchar(30)  not null,
  Semester integer not null,
  primary key(MatrNr)
);

create table Professoren
(
  PersNr   integer not null,
  Name     varchar(30)  not null,
  Rang     char(2) not null,
  Raum     integer not null,
  primary key (PersNr)
  );

create table Assistenten
(
  PersNr   integer not null,
  Name     varchar(30) not null,
  Fachgebiet     varchar(30) not null,
  Boss     integer not null,
  primary key (PersNr)
);

create table Vorlesungen
(
  VorlNr   integer not null,
  Titel    varchar(30)  not null,
  SWS      integer not null,
  gelesenVon     integer not null,
  primary key (VorlNr)
);

create table hoeren
(
  s_MatrNr   integer not null,
  v_VorlNr   integer not null,
  primary key  (s_MatrNr, v_VorlNr)
);

create table voraussetzen
(
  Vorgaenger     integer not null,
  Nachfolger     integer not null,
  primary key (Vorgaenger, Nachfolger)
);

create table pruefen
(
  MatrNr   integer not null,
  VorlNr   integer not null,
  PersNr   integer not null,
  Note     numeric(2,1) not null,
  primary key  (MatrNr, VorlNr)
);