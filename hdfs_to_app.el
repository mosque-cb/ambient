(defun multi(event tt name)
  (atadd event 
         (add (unixtime) tt)
         (lambda() (progn
                     (print (concat 'rm 'SPACE name))
                     (system (concat 'rm
                                     'SPACE
                                     name))
                     (system (concat 'rm
                                     'SPACE
                                     name
                                     (storage -filter)))))))

(defun wrapno(no)
  (if (eq (strlen no) 4)
      (concat (storage 0) no)
    (if (eq (strlen no) 3)
        (concat (storage 00) no)
      (if (eq (strlen no) 2)
          (concat (storage 000) no)
        (if (eq (strlen no) 1)
            (concat (storage 00000) no)
          nil)))))

(defun main(no event)
  (if (eq no 0)
      nil
    (progn
      (atpoll  event nil)
      (print (concat 'file 'SPACE  no))
      (if (eq (ftell (concat (storage part-)
                             (wrapno (itoa no))
                             (storage -R)))
              nil)
          (progn
            (system (concat (storage ~/hadoop_yq_guiji/bin/../bin/hadoop fs  -get   /app/lbs/lbs-guiji/recommend/user.status.off.res.incre/20161230/part-)
                            (wrapno (itoa no))
                            (storage -R.gz . )))
            (system (concat (storage gunzip part-)
                            (wrapno (itoa no))
                            (storage -R.gz)))
            (system (concat (storage cat part-)
                            (wrapno (itoa no))
                            (storage -R | sh staypoint_importer_map.sh  | ./staypoint_importer_executable 1 > input  2 >&1  )))
            (system (concat  (storage cat part-)
                             (wrapno (itoa no))
                             (storage -R)
                             (storage  | ./a.out  filter_with_ardb.el  1 > ) 
                             (storage part-)
                             (wrapno (itoa no))
                             (storage -R-filter)
                             'SPACE))
            (system (concat (storage cat part-)
                            (wrapno (itoa no))
                            (storage -R-filter | sh staypoint_importer_map.sh  | ./staypoint_importer_cleaner 1 > input  2 >&1  )))
            (print (concat 'file 'SPACE  no 'SPACE 'done))
            (multi event
                   3600
                   (concat (storage part-)
                           (wrapno (itoa no))
                           (storage -R))))
        (print (concat 'file 'SPACE  no 'SPACE 'skip)))
      (main (minus no 1)
            event))))

(main (atoi (strip (stdin)))
      (atcreate))
