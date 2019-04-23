(defun caar (lst)
  (car (car lst)))

(defun cddr (lst)
  (cdr (cdr lst)))

(defun cadr (lst)
  (car (cdr lst)))

(defun cdar (lst)
  (cdr (car lst)))

(defun caddr (lst)
  (car (cddr lst)))

(defun printpos (n)
  (if  (eq   n  nil)
      nil
    (progn
      (print  'pos)
      (printlst  (car  n) )
      (printpos (cdr n) ))))

(defun printlst (n)
  (if  (eq   n  nil)
      nil
    (progn
      (print    (car  n) )
      (printlst (cdr n) ))))

(defun evprogn ()
  (if  (eq   (geq  args)  nil)
      nil
    (progn
      (progn   (seq  val  (car  (geq args))))
      (progn   (seq  args  (cdr  (geq args))))
      (evprogn ))))

(defun  evif ()
  (progn
    (progn  (seq  expr  
                   (remove_ptr clink)))
    (if  val
        (progn
          (progn  (seq  expr  
                         (caddr 
                          (geq expr))))
          (wrapeval))
      (progn
        (progn  (seq  expr  
                       (cadr 
                        (cddr 
                         (geq expr)))))
        (wrapeval)))))

(defun  formstruct (lst)
  (if  (eq  lst  nil)
      nil
    (if  (atom lst)
        lst
      (cons  (formstruct (car lst))
             (formstruct (cdr lst))))))

(defun  wrapdefun (lst )
  (progn
    (progn  (seq  globaldefun  
                   (cons   lst  
                           (geq   globaldefun))))
    (print  'wrapdefun)))

(defun  funp(name global)
  (if  (eq  global  nil)
      0
    (if  (eq  name  (caar  global))
        1
      (funp  name  (cdr  global)))))

(defun  findexpr(name global)
  (if  (eq  name  
            (caar  global))
      (cdar  global)
    (findexpr  name  
               (cdr  global))))

(defun  bindvars  (arg value env)
  (cons
   (bindvarshelp arg value)
   env))

(defun  bindvarshelp  (arg value )
  (if  (eq  arg  nil)
      nil
    (cons  ( cons  (car arg)  
                   (cons (car value)
                         nil))
           (bindvarshelp  (cdr  arg) (cdr value)))))

(defun  varfind (arg env)
  (if  (eq  env  nil)
      0
    (if  (eq  arg  
              (car  (car env)))
        (progn
          (progn (seq midx  
                       (cadr (car env))))
          1)
      (varfind arg  
               (cdr env)))))

(defun  varfetch (arg env)
  (if  (eq  arg  
            (car  (car env)))
      (cadr  
       (car env))
    (varfetch arg  
              (cdr env))))

(defun  findvar  (arg  env)
  (if  (varfind   arg    
                  (car env))
      (geq midx)
    (findvar  arg  
              (cdr env))))

(defun   evargslast()
  (progn
    (progn  (seq  expr  
                   (remove_ptr  clink)))
    (if  (eq  (geq expr)  
              'evprogn)
        (progn
          (evprogn)
          (popjreturn))
      (if  (eq  (geq expr)  
                'printpos)
          (progn
            (progn  (seq val 
                          (printpos  (geq args))))
            (popjreturn))
        (if  (eq  (geq expr)  
                  'printlst)
            (progn
              (progn  (seq val 
                            (printlst  (geq args))))
              (popjreturn))
          (if  (primitivep  (geq  expr))
              (progn
                (progn  (seq val 
                              (primitive (geq expr) 
                                         (geq args))))
                (popjreturn))
            (sapply)
            ))))))

(defun  sapply ()
  (progn    
    (progn  (seq  expr  
                   (findexpr (geq expr) 
                             (geq globaldefun))))
    (progn  (seq  env 
                   (bindvars         (car (geq expr))    
                                     (geq args)
                                     (geq  env))))
    (progn  (seq  expr 
                   (cadr  (geq expr))))
    (wrapeval)))

(defun   evargscombi ()
  (progn
    (progn  (seq  args 
                   (remove_ptr clink)))
    (progn  (join args  
                  (geq val)))
    (progn  (seq  env 
                   (remove_ptr  clink)))
    (progn  (seq  expr  
                   (remove_ptr  clink)))
    (evargs)))

(defun   evargs ()
  (progn
    (if  (eq expr nil)
        (evargslast)
      (progn
        (progn   (add_long   clink  
                          (cdr (geq expr))))
        (progn   (add_long   clink  
                          (geq env)))
        (progn   (add_long   clink  
                          (geq args)))
        (progn   (add_long   clink  
                          'evargscombi))
        (progn  (seq  expr 
                       (car  (geq expr))))
        (wrapeval)))))

(defun  popjreturn ()
  (progn
    (progn  (seq  expr 
                   (remove_ptr clink)))
    (funcall  (geq expr))
    ))

(defun   wrapeval ()
  (progn
    (if  (digitp  (geq expr ))
        (progn
          (progn  (seq  val  
                         (geq expr)))
          (popjreturn))
      (if  (eq  (geq expr) nil)
          (progn
            (progn  (seq  val  nil))
            (popjreturn))
        (if  (charp  (geq expr))
            (progn
              (progn  (seq  val 
                             (findvar (geq expr)
                                      (geq  env))))
              (popjreturn))
          (if  (eq (car (geq expr))  'quote)
              (progn
                (progn  (seq  val 
                               (cadr  (geq expr))))
                (popjreturn))
            (if  (eq (car (geq expr))  'if)
                (progn
                  (progn   (add_long   clink  
                                    (geq expr)))
                  (progn   (add_long   clink  
                                    'evif))
                  (progn  (seq  expr   
                                 (cadr  (geq expr))))
                  (wrapeval))
              (if  (eq (car (geq expr))  'defun)
                  (wrapdefun  (cdr (geq expr )))
                (if  (eq (car (geq expr))  'progn)
                    (progn
                      (progn   (add_long   clink  
                                        'evprogn))
                      (progn  (seq  expr   
                                     (cdr (geq expr))))
                      (progn  (seq  args  
                                     nil))
                      (evargs))
                  (progn
                    (progn   (add_long   clink  
                                      (car  (geq expr))))
                    (progn  (seq  expr   
                                   (cdr (geq expr))))
                    (progn  (seq  args  
                                   nil))
                    (evargs)))))))))))

(defun  generand  (count range)
  (if  (eq  count  0)
      nil
    (cons    (random  range)
             (generand  (minus  count  1)
                        range))))

(defun once  ( )
  (progn 
    (display )
    (seq  env     nil)
    (seq  zencode  nil)
    (seq  zendata  nil)
    (seq midx  nil)
    (seq  globaldefun  nil)

    (main   basicdefun )
    (main   basicapply )

    (seq  env     nil)
    (seq  zencode  nil)
    (seq  zendata  nil)
    (seq midx  nil)
    (seq  globaldefun  nil)
    (display)
    (print  'hell)))

(defun  autotest(num)
  (progn
    (once)
    (print (concat (quote times is ) num))
    (autotest (add num 1))))

(defun  main (lst  )
  (if  (eq  lst  nil)
      nil
    (progn
      (seq  expr  (car  lst))
      (print   (wrapeval))
      (main (cdr lst)  ))))

(seq  val  nil)
(seq  expr  nil)
(seq  env  nil)
(seq  args  nil)
(seq  clink  nil)
(seq midx  nil)
(seq  primop  '(cons car cdr add minus mod random))
(seq  globaldefun  nil)
(define  basicdefun   '( 
                        (defun  generand  (count range)
                          (if  (eq  count  0)
                              nil
                            (cons    (random  range)
                                     (generand  (minus  count  1)
                                                range))))

                        (defun  value_x  (position)
                          (car  position))

                        (defun  value_y  (position)
                          (car (cdr  position)))

                        (defun  form_pos  (  x  y)
                          (cons  x  (cons  y 
                                           nil)))

                        (defun  add_x_pos (position)
                          (form_pos (add  (value_x position)  
                                          1)
                                    (value_y  position)))

                        (defun  minus_x_pos (position)
                          (form_pos (minus  (value_x position)  
                                            1)
                                    (value_y  position)))

                        (defun  add_y_pos (position)
                          (form_pos (value_x position) 
                                    (add (value_y  position)
                                         1)))

                        (defun  minus_y_pos (position)
                          (form_pos (value_x position) 
                                    (minus (value_y  position)
                                           1)))

                        (defun  up_y (n path)
                          (if (eq  n  0)
                              path
                            (up_y  (minus  n 
                                           1)
                                   (cons  (add_y_pos (car path))
                                          path))))

                        (defun  down_y (n path)
                          (if (eq  n  0)
                              path
                            (down_y  (minus  n 
                                             1)
                                     (cons  (minus_y_pos (car path))
                                            path))))

                        (defun  up_x (n path)
                          (if (eq  n  0)
                              path
                            (up_x  (minus  n  
                                           1)
                                   (cons  (add_x_pos (car path))
                                          path))))

                        (defun  down_x (n path)
                          (if (eq  n  0)
                              path
                            (down_x  (minus  n 
                                             1)
                                     (cons  (minus_x_pos (car path))
                                            path))))

                        (defun  wrap_helpery  (path eggs)
                          (helpery  (value_y (car  path))
                                    (value_y  (car eggs))
                                    path))

                        (defun   helpery  (fro  toy path )
                          (if  (big  fro  toy)                
                              (down_y  (minus fro toy)
                                       path)
                            (up_y  (minus toy fro ) 
                                   path)))

                        (defun move_left (from  )
                          (down_x  (minus (value_x (car from)) 
                                          1)
                                   from ))

                        (defun move_right (from )
                          (up_x  (minus 10
                                        (value_x (car from)) )
                                 from ))

                        (defun  collision (  path eggs direct  result)
                          (strategy           path
                                              (cdr eggs)
                                              direct
                                              (cons  (car path) result)))


                        (defun   wrap_helperx  ( path eggs  direct result)
                          (helperx  (value_x  (car  path))
                                    (value_x  (car  eggs))
                                    path
                                    eggs
                                    direct
                                    result))

                        (defun   helperx  (fromx  tox path eggs  direct result)
                          (if (eq  fromx  tox)
                              (collision   path  
                                           eggs 
                                           direct
                                           result)
                            (if  (big  fromx  tox)                
                                (collision	      (down_x  (minus fromx tox) 
                                                           path) 
                                                  eggs 
                                                  direct
                                                  result)
                              (collision	    (up_x  (minus tox fromx ) 
                                                       path )
                                                eggs
                                                direct
                                                result))))

                        (defun  form_eggs (xpath ypath)
                          (if  (eq  xpath nil)
                              nil
                            (cons  (form_pos (car xpath)
                                             (car ypath))
                                   (form_eggs  (cdr xpath)
                                               (cdr  ypath)))))

                        (defun  wrap_strategy (eggs)
                          (progn
                            (print  'eggs)
                            (printpos eggs)
                            (print 'path)
                            (printpos  (strategy  (cons (car eggs) 
                                                        nil)
                                                  (cdr eggs) 
                                                  0
                                                  (cons (car eggs) 
                                                        nil)))))

                        (defun  snake  ()
                          (wrap_strategy (form_eggs 
                                          (generand  10  10)
                                          (generand  10  10)
                                          )))

                        (defun  strategy (path  eggs direct result)
                          (if  (eq  eggs  nil)
                              result
                            (if  (eq  (value_y  (car path))        
                                      (value_y  (car eggs)))
                                (if  (big  (value_x  (car path))        
                                           (value_x  (car eggs)))
                                    (if  (eq  direct  0)
                                        (collision		(down_x  (minus   (value_x  (car path))
                                                                          (value_x  (car eggs)))
                                                                 path)
                                                        eggs
                                                        direct
                                                        result)
                                      (progn
                                        (print 'back)
                                        (if  (eq  (value_y  (car path))  10)
                                            (strategy  (down_y  1      (move_right  path))
                                                       eggs
                                                       (minus  1 direct)
                                                       result)
                                          (strategy  (up_y  1      (move_right  path))
                                                     eggs
                                                     (minus  1 direct)
                                                     result))))
                                  (if  (eq  direct  0)
                                      (progn
                                        (print 'back)
                                        (if  (eq  (value_y  (car path))  10)
                                            (strategy  (down_y  1      (move_left  path))
                                                       eggs
                                                       (minus  1 direct)
                                                       result)
                                          (strategy  (up_y  1      (move_left  path))
                                                     eggs
                                                     (minus  1 direct)
                                                     result)))
                                    (collision		(up_x    (minus   (value_x  (car eggs))
                                                                      (value_x  (car path)))
                                                             path)
                                                    eggs
                                                    direct
                                                    result)
                                    ))
                              (if  (eq  direct  0)
                                  (wrap_helperx  (wrap_helpery      (move_left  path)
                                                                    eggs)
                                                 eggs
                                                 (minus  1 direct)
                                                 result)
                                (wrap_helperx  (wrap_helpery       (move_right  path)
                                                                   eggs)
                                               eggs
                                               (minus  1 direct)
                                               result)))))
                        ))


(define  basicapply    '( 
                         (snake)
                         ))

(define  basicshow    'hello)
(print   'initobject)
(once)
(autotest  0)
