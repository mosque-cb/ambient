(defun fibs(n)
  (if (eq  n 1)
      1
    (if (eq n 2)
        2
      (add (fibs  (minus n 1))
           (fibs  (minus n 2))))))

(defun routine(name)
  (progn
    (print (concat (itoa (fibs (random 15)))
                   'SPACE
                   name))
    (routine name)))

(setq this (pcreate 1 'routine 'this))
(setq that (pcreate 1 'routine 'that))

(comment (pjoin this))
(comment (pjoin that))

(pjoin 'nothing)
