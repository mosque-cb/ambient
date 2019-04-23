(defun wrapno(no)
  (timetodata (minus (unixtime)
                     (mul no 24  60 60))))

(defun main(no)
  (if (eq no 60)
      nil
    (progn
      (print (concat 'file 'SPACE  no))
      (system (print (concat (quote /home/map/tools/hadoop_lbs-traj/bin/hadoop fs -mkdir )
                             (quote /app/lbs/traj/search/seman_output_ios/)
                             (wrapno no))))
      (system (print (concat (quote /home/map/tools/hadoop_lbs-traj/bin/hadoop distcp -D mapred.job.priority=VERY_HIGH -su cm_rank,RaNKRtyO hdfs://yq01-wutai-hdfs.dmop.company.com:54310/app/ecom/cm/sh/mizong/lbs_semantic/)
                             (wrapno no)
                             (quote /ios /app/lbs/traj/search/seman_output_ios/)
                             (wrapno no))))
      (print (concat 'file 'SPACE  no 'SPACE 'done))
      (sleep 100)
      (main (add no 1)))))

(main (atoi (strip (stdin))))
