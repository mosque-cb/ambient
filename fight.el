(defun etl(item)
  (if (eq (cdr item ) nil)
      (car item)
    (etl (cdr item))))

(defun decide_path(path lst xml)
  (if (eq path nil)
      (decide_path (car lst)
                   (cdr lst)
                   xml)
    (if (not (eq (find path 'DONE) nil))
        (decide_path (car lst)
                     (cdr lst)
                     xml)
      (if (eq (find path '.) nil)
          (expoler path
                   xml
                   lst)
        (progn
          (print path)
          (decide_path (car lst)
                       (cdr lst)
                       xml))))))

(defun find_path(lst xml)
  (if  (eq lst nil)
      (print 'over)
    (decide_path   (car lst)
                   (cdr lst)
                   xml)))

(defun get_list(root xml)
  (progn
    (system (concat (storage hadoop fs  -conf  )
                    xml
                    'SPACE
                    (storage  -ls )
                    root
                    'SPACE
                    (storage  | tail -n  50  ->)
                    (concat xml  'owner)))
    (fload (concat xml  'owner))))

(defun avoid(root item a b)
  (if (eq item 'items)
      (appdixx 
       root
       (cdr a)
       b)
    (if (eq item root)
        (appdixx 
         root
         (cdr a)
         b)
      (cons item
            (appdixx 
             root
             (cdr a)
             b)))))

(defun appdixx(root a b)
  (if (eq a nil)
      b
    (avoid root 
           (etl (spacesplit (car a)))
           a
           b)))

(defun wraplist(root a b)
  (if (eq a nil)
      b
    (appdixx 
     root
     a
     b)))

(defun wrapexpoler(root xml father)
  (find_path (wraplist 
              root
              (entersplit 
               (get_list root xml))
              father)
             xml))

(defun expoler(root xml father)
  (if  (eq father nil)
      (print 'over)
    (if (eq root nil)
        (find_path father xml)
      (wrapexpoler root xml father))))

(defun  waiting(left right)
  (progn
    (pjoin left)
    (pjoin right)
    (print 'done)
    (print (timestring))
    (dispatch)))

(defun dispatch()
  (waiting 
   (pcreate
    1 
    'wrapexpoler '/app/dt/ups 'a.xml nil)
   (pcreate 
    1
    'wrapexpoler '/app/dt/ups 'b.xml nil)))

(dispatch)
