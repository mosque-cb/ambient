(defun etl(item)
  (if (eq (cdr item ) nil)
      (car item)
    (etl (cdr item))))

(defun get_list(root)
  (progn
    (system (concat (storage  ls )
                    root
                    'SPACE
                    (storage  >owner)))
    (fload 'owner)))

(defun terminal(path)
  (system (concat (storage python dump.py  )  
                  path 
                  (storage >> building ))))

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
       (concat  root (car a) (storage /))
       (entersplit (get_list (concat  root (car a)))))
      (scandir root (cdr a)))))

(defun wrapexpoler(root)
  (scandir 
   root
   (print (entersplit 
           (get_list root)))))

(wrapexpoler (strip (stdin)))
