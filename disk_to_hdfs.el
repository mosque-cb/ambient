(defun wrapno(no)
  (timetodata (minus (unixtime)
                     (mul no 24  60 60))))

(defun main(no)
  (if (eq no 60)
      nil
    (progn
      (print (concat 'file 'SPACE  no))
      (system (print (concat (storage /home/map/tools/hadoop_lbs-traj/bin/hadoop  fs  -mkdir  /app/lbs/traj/search/seman_output_ios/)
                             (wrapno no))))
      (system (print (concat (storage /home/map/tools/hadoop_lbs-traj/bin/hadoop  fs  -put )
                             (wrapno no)
                             (storage /ios* )
                             (storage  /app/lbs/traj/search/seman_output_ios/)
                             (wrapno no))))
      (print (concat 'file 'SPACE  no 'SPACE 'done))
      (sleep 100)
      (main (add no 1)))))

(main (atoi (strip (stdin))))
