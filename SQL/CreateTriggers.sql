create or replace function fn_sort_stud() returns trigger as $sort_student$
      DECLARE 
      	arr text[];
      BEGIN
        arr := ARRAY(SELECT _EMAIL FROM _Student ORDER BY _FIO);
        update _Student set _NUMBER=array_position(arr,_EMAIL);
        return NEW;
      END;
    $sort_student$ language plpgsql;
    
drop trigger if exists sort_student1 on _Student;
create trigger sort_student1 after insert on _Student
      for each row execute procedure fn_sort_stud(); 
      
drop trigger if exists sort_student2 on _Student; 
create trigger sort_student2 after delete on _Student
      for each row execute procedure fn_sort_stud();
      
create or replace function fn_del_stud() returns trigger as $del_student$
      begin
        delete from _Student where _Student._GROUP=old._NAME;
        return old;
      end;
    $del_student$ language plpgsql;
    
drop trigger if exists del_student on _Group; 
create trigger del_student before delete on _Group
      for each row execute procedure fn_del_stud();
      
create or replace function fn_del_tests() returns trigger as $del_tests$
      begin
        delete from _SavedTest where _SavedTest._EMAIL_STUDENT=old._EMAIL;
        return old;
      end;
    $del_tests$ language plpgsql;
    
drop trigger if exists del_tests on _Student; 
create trigger del_tests before delete on _Student
      for each row execute procedure fn_del_tests();  
