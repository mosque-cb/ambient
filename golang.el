(defun worker(n)
  (add n 1))

(defun begin(n)
  n)

(defun golang(thread n)
  (if (eq n 0)
      thread
    (golang
     (pcreate (unixtime)
              'worker
              (pjoin thread))
     (minus n 1))))

(defun  autotest(start begin)
  (progn
    (display)
    (print (pjoin (golang (pcreate (unixtime) 'begin 1)
                          (print (random 1024000)))))
    (print (concat 'running 'SPACE start 'SPACE (minus (unixtime) begin)))
    (autotest (add start 1) (unixtime))))

(autotest 0 (unixtime))
(pjoin (pcreate 1 'autotest  0))



