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
                                     (quote -filter)))))))

(defun wrapno(no)
  (if (eq (strlen no) 4)
      (concat (quote 0) no)
    (if (eq (strlen no) 3)
        (concat (quote 00) no)
      (if (eq (strlen no) 2)
          (concat (quote 000) no)
        (if (eq (strlen no) 1)
            (concat (quote 00000) no)
          nil)))))

(defun main(no event)
  (if (eq no 0)
      nil
    (progn
      (atpoll  event nil)
      (print (concat 'file 'SPACE  no))
      (if (eq (ftell (concat (quote part-)
                             (wrapno (itoa no))
                             (quote -R)))
              nil)
          (progn
            (system (concat (quote ~/hadoop_yq_guiji/bin/../bin/hadoop fs  -get   /app/lbs/lbs-guiji/recommend/user.status.off.res.incre/20161230/part-)
                            (wrapno (itoa no))
                            (quote -R.gz . )))
            (system (concat (quote gunzip part-)
                            (wrapno (itoa no))
                            (quote -R.gz)))
            (system (concat (quote cat part-)
                            (wrapno (itoa no))
                            (quote -R | sh staypoint_importer_map.sh  | ./staypoint_importer_executable 1 > input  2 >&1  )))
            (system (concat  (quote cat part-)
                             (wrapno (itoa no))
                             (quote -R)
                             (quote  | ./a.out  filter_with_ardb.el  1 > ) 
                             (quote part-)
                             (wrapno (itoa no))
                             (quote -R-filter)
                             'SPACE))
            (system (concat (quote cat part-)
                            (wrapno (itoa no))
                            (quote -R-filter | sh staypoint_importer_map.sh  | ./staypoint_importer_cleaner 1 > input  2 >&1  )))
            (print (concat 'file 'SPACE  no 'SPACE 'done))
            (multi event
                   3600
                   (concat (quote part-)
                           (wrapno (itoa no))
                           (quote -R))))
        (print (concat 'file 'SPACE  no 'SPACE 'skip)))
      (main (minus no 1)
            event))))

(main (atoi (strip (stdin)))
      (atcreate))
