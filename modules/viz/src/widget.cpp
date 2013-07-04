#include "precomp.hpp"

class temp_viz::Widget::Impl
{
public:
    vtkSmartPointer<vtkLODActor> actor;

    Impl() : actor(vtkSmartPointer<vtkLODActor>::New()) {}

    void setColor(const Color& color)
    {
        Color c = vtkcolor(color);
        actor->GetMapper ()->ScalarVisibilityOff ();
        actor->GetProperty ()->SetColor (c.val);
        actor->GetProperty ()->SetEdgeColor (c.val);
        actor->GetProperty ()->SetAmbient (0.8);
        actor->GetProperty ()->SetDiffuse (0.8);
        actor->GetProperty ()->SetSpecular (0.8);
        actor->GetProperty ()->SetLighting (0);
        actor->Modified ();
    }

    void setPose(const Affine3f& pose)
    {
        vtkSmartPointer<vtkMatrix4x4> matrix = convertToVtkMatrix(pose.matrix);
        actor->SetUserMatrix (matrix);
        actor->Modified ();
    }

    void updatePose(const Affine3f& pose)
    {
        vtkSmartPointer<vtkMatrix4x4> matrix = actor->GetUserMatrix();
        Matx44f matrix_cv = convertToMatx(matrix);

        Affine3f updated_pose = pose * Affine3f(matrix_cv);
        matrix = convertToVtkMatrix(updated_pose.matrix);

        actor->SetUserMatrix (matrix);
        actor->Modified ();
    }

    Affine3f getPose() const
    {
        vtkSmartPointer<vtkMatrix4x4> matrix = actor->GetUserMatrix();
        Matx44f matrix_cv = convertToMatx(matrix);
        return Affine3f(matrix_cv);
    }

protected:

    static vtkSmartPointer<vtkMatrix4x4> convertToVtkMatrix (const cv::Matx44f& m)
    {
        vtkSmartPointer<vtkMatrix4x4> vtk_matrix = vtkSmartPointer<vtkMatrix4x4>::New ();
        for (int i = 0; i < 4; i++)
            for (int k = 0; k < 4; k++)
                vtk_matrix->SetElement(i, k, m(i, k));
        return vtk_matrix;
    }

    static cv::Matx44f convertToMatx(const vtkSmartPointer<vtkMatrix4x4>& vtk_matrix)
    {
        cv::Matx44f m;
        for (int i = 0; i < 4; i++)
            for (int k = 0; k < 4; k++)
                m(i, k) = vtk_matrix->GetElement (i, k);
        return m;
    }
};


///////////////////////////////////////////////////////////////////////////////////////////////
/// stream accessor implementaion

vtkSmartPointer<vtkLODActor> temp_viz::WidgetAccessor::getActor(const Widget& widget)
{
    return widget.impl_->actor;
}

///////////////////////////////////////////////////////////////////////////////////////////////
/// widget implementaion

temp_viz::Widget::Widget()
{
    impl_ = new Impl();
}

temp_viz::Widget::Widget(const Widget& other)
{
    impl_ = other.impl_;
}

temp_viz::Widget& temp_viz::Widget::operator =(const Widget &other)
{
    if (this != &other)
        impl_ = other.impl_;
    return *this;
}

void temp_viz::Widget::copyTo(Widget& /*dst*/)
{
    // TODO Deep copy the data if there is any
}

void temp_viz::Widget::setColor(const Color& color) { impl_->setColor(color); }
void temp_viz::Widget::setPose(const Affine3f& pose) { impl_->setPose(pose); }
void temp_viz::Widget::updatePose(const Affine3f& pose) { impl_->updatePose(pose); }
temp_viz::Affine3f temp_viz::Widget::getPose() const { return impl_->getPose(); }



