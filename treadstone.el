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
  (if  (eq   (whole  args)  nil)
      nil
    (progn
      (progn   (setq  val  (car  (whole args))))
      (progn   (setq  args  (cdr  (whole args))))
      (evprogn ))))

(defun  evif ()
  (progn
    (progn  (setq  expr  
                   (eject clink)))
    (if  val
        (progn
          (progn  (setq  expr  
                         (caddr 
                          (whole expr))))
          (wrapeval))
      (progn
        (progn  (setq  expr  
                       (cadr 
                        (cddr 
                         (whole expr)))))
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
    (progn  (setq  globaldefun  
                   (cons   lst  
                           (whole   globaldefun))))
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
          (progn (setq midx  
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
      (whole midx)
    (findvar  arg  
              (cdr env))))

(defun   evargslast()
  (progn
    (progn  (setq  expr  
                   (eject  clink)))
    (if  (eq  (whole expr)  
              'evprogn)
        (progn
          (evprogn)
          (popjreturn))
      (if  (eq  (whole expr)  
                'printpos)
          (progn
            (progn  (setq val 
                          (printpos  (whole args))))
            (popjreturn))
        (if  (eq  (whole expr)  
                  'printlst)
            (progn
              (progn  (setq val 
                            (printlst  (whole args))))
              (popjreturn))
          (if  (primitivep  (whole  expr))
              (progn
                (progn  (setq val 
                              (primitive (whole expr) 
                                         (whole args))))
                (popjreturn))
            (sapply)
            ))))))

(defun  sapply ()
  (progn    
    (progn  (setq  expr  
                   (findexpr (whole expr) 
                             (whole globaldefun))))
    (progn  (setq  env 
                   (bindvars         (car (whole expr))    
                                     (whole args)
                                     (whole  env))))
    (progn  (setq  expr 
                   (cadr  (whole expr))))
    (wrapeval)))

(defun   evargscombi ()
  (progn
    (progn  (setq  args 
                   (eject clink)))
    (progn  (join args  
                  (whole val)))
    (progn  (setq  env 
                   (eject  clink)))
    (progn  (setq  expr  
                   (eject  clink)))
    (evargs)))

(defun   evargs ()
  (progn
    (if  (eq expr nil)
        (evargslast)
      (progn
        (progn   (press   clink  
                          (cdr (whole expr))))
        (progn   (press   clink  
                          (whole env)))
        (progn   (press   clink  
                          (whole args)))
        (progn   (press   clink  
                          'evargscombi))
        (progn  (setq  expr 
                       (car  (whole expr))))
        (wrapeval)))))

(defun  popjreturn ()
  (progn
    (progn  (setq  expr 
                   (eject clink)))
    (funcall  (whole expr))
    ))

(defun   wrapeval ()
  (progn
    (if  (digitp  (whole expr ))
        (progn
          (progn  (setq  val  
                         (whole expr)))
          (popjreturn))
      (if  (eq  (whole expr) nil)
          (progn
            (progn  (setq  val  nil))
            (popjreturn))
        (if  (charp  (whole expr))
            (progn
              (progn  (setq  val 
                             (findvar (whole expr)
                                      (whole  env))))
              (popjreturn))
          (if  (eq (car (whole expr))  'quote)
              (progn
                (progn  (setq  val 
                               (cadr  (whole expr))))
                (popjreturn))
            (if  (eq (car (whole expr))  'if)
                (progn
                  (progn   (press   clink  
                                    (whole expr)))
                  (progn   (press   clink  
                                    'evif))
                  (progn  (setq  expr   
                                 (cadr  (whole expr))))
                  (wrapeval))
              (if  (eq (car (whole expr))  'defun)
                  (wrapdefun  (cdr (whole expr )))
                (if  (eq (car (whole expr))  'progn)
                    (progn
                      (progn   (press   clink  
                                        'evprogn))
                      (progn  (setq  expr   
                                     (cdr (whole expr))))
                      (progn  (setq  args  
                                     nil))
                      (evargs))
                  (progn
                    (progn   (press   clink  
                                      (car  (whole expr))))
                    (progn  (setq  expr   
                                   (cdr (whole expr))))
                    (progn  (setq  args  
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
    (setq  env     nil)
    (setq  zencode  nil)
    (setq  zendata  nil)
    (setq midx  nil)
    (setq  globaldefun  nil)

    (main   basicdefun )
    (main   basicapply )

    (setq  env     nil)
    (setq  zencode  nil)
    (setq  zendata  nil)
    (setq midx  nil)
    (setq  globaldefun  nil)
    (display)
    (print  'hell)))

(defun  autotest(num)
  (progn
    (once)
    (print (concat (storage times is ) num))
    (autotest (add num 1))))

(defun  main (lst  )
  (if  (eq  lst  nil)
      nil
    (progn
      (setq  expr  (car  lst))
      (print   (wrapeval))
      (main (cdr lst)  ))))

(setq  val  nil)
(setq  expr  nil)
(setq  env  nil)
(setq  args  nil)
(setq  clink  nil)
(setq midx  nil)
(setq  primop  '(cons car cdr add minus mod random))
(setq  globaldefun  nil)
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
