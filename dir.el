(defun etl(item)
  (if (eq (cdr item ) nil)
      (car item)
    (etl (cdr item))))

(defun get_list(root)
  (progn
    (system (concat (quote  ls )
                    root
                    'SPACE
                    (quote  >owner)))
    (fload 'owner)))

(defun terminal(path)
  (system (concat (quote python dump.py  )  
                  path 
                  (quote >> building ))))

(defun scanfile(root a)
  (if (eq a nil)
      nil
    (progn
      (terminal (concat root (car a)))
      (scanfile root (cdr a)))))

(defun scandir(root a)
  (if (eq a nil)
      nil
    (progn
      (scanfile
       (concat  root (car a) (quote /))
       (entersplit (get_list (concat  root (car a)))))
      (scandir root (cdr a)))))

(defun wrapexpoler(root)
  (scandir 
   root
   (print (entersplit 
           (get_list root)))))

(wrapexpoler (strip (stdin)))
